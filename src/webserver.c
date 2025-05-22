#include <stdio.h>
#include <string.h>
#include "esp_log.h"
#include "esp_http_server.h"
#include "spiffs_init.h"
#include "config.h"
#include "harmonics.h"
#include "recorder.h"

static const char *TAG = "WEBSERVER";
static httpd_handle_t server_handle = NULL;

/** 
 * @brief Liest eine Datei aus SPIFFS und sendet sie als HTTP-Antwort.
 */
static esp_err_t serve_file(httpd_req_t *req, const char *path, const char *content_type) {
    ESP_LOGI(TAG, "Serve %s", path);
    FILE *f = fopen(path, "r");
    if (!f) {
        ESP_LOGE(TAG, "File not found: %s", path);
        return httpd_resp_send_404(req);
    }
    fseek(f, 0, SEEK_END);
    long size = ftell(f);
    fseek(f, 0, SEEK_SET);
    char *buf = malloc(size);
    if (!buf) {
        fclose(f);
        ESP_LOGE(TAG, "No memory to serve %s", path);
        return httpd_resp_send_500(req);
    }
    fread(buf, 1, size, f);
    fclose(f);

    httpd_resp_set_type(req, content_type);
    esp_err_t err = httpd_resp_send(req, buf, size);
    free(buf);
    return err;
}

/** Handler für „/“ → index.html */
esp_err_t index_handler(httpd_req_t *req) {
    return serve_file(req, "/spiffs/index.html", "text/html");
}

/** Handler für „/harmonics“ → harmonics.json über harmonics_handler */
esp_err_t harmonics_page_handler(httpd_req_t *req) {
    return serve_file(req, "/spiffs/harmonics.html", "text/html");
}

/** WebSocket-Endpunkt für Scope-Daten */
esp_err_t ws_scope_handler(httpd_req_t *req) {
    ESP_LOGI(TAG, "WS_SCOPE: New frame");
    if (req->method == HTTP_GET) {
        // Connection upgrade
        ESP_LOGI(TAG, "WS_SCOPE: Handshake");
        return ESP_OK;
    }
    // Payload empfangen
    httpd_ws_frame_t rcv_frame;
    memset(&rcv_frame, 0, sizeof(rcv_frame));
    esp_err_t ret = httpd_ws_recv_frame(req, &rcv_frame, 0);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "WS_SCOPE: recv error %d", ret);
        return ret;
    }
    if (rcv_frame.len) {
        // Wir ignorieren Inhalt – senden immer aktuellen Scope
        extern esp_err_t send_scope_frame(httpd_req_t*);
        return send_scope_frame(req);
    }
    return ESP_OK;
}

/** WebSocket-Endpunkt für Harmonics-Daten */
esp_err_t ws_harmonics_handler(httpd_req_t *req) {
    ESP_LOGI(TAG, "WS_HARM: New frame");
    if (req->method == HTTP_GET) {
        ESP_LOGI(TAG, "WS_HARM: Handshake");
        return ESP_OK;
    }
    // Payload ignorieren, direkt senden
    return ws_harmonics_handler(req);  // delegiert an harmonics.c
}

/** Download-Handler (interleaved V,I as float32) */
esp_err_t download_page_handler(httpd_req_t *req) {
    return download_handler(req);
}

httpd_handle_t webserver_start(void) {
    if (server_handle) {
        ESP_LOGW(TAG, "Server already running");
        return server_handle;
    }

    // Mount SPIFFS für statische Dateien
    init_spiffs();

    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    config.max_uri_handlers = 8;
    esp_err_t err = httpd_start(&server_handle, &config);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to start server: %s", esp_err_to_name(err));
        return NULL;
    }

    // URI-Handler registrieren
    httpd_register_uri_handler(server_handle, &(httpd_uri_t){
        .uri      = "/", 
        .method   = HTTP_GET,
        .handler  = index_handler
    });
    httpd_register_uri_handler(server_handle, &(httpd_uri_t){
        .uri      = "/harmonics.html",
        .method   = HTTP_GET,
        .handler  = harmonics_page_handler
    });
    httpd_register_uri_handler(server_handle, &(httpd_uri_t){
        .uri      = "/harmonics",
        .method   = HTTP_GET,
        .handler  = harmonics_handler
    });
    httpd_register_uri_handler(server_handle, &(httpd_uri_t){
        .uri        = "/ws_scope",
        .method     = HTTP_GET,
        .handler    = ws_scope_handler,
        .is_websocket = true
    });
    httpd_register_uri_handler(server_handle, &(httpd_uri_t){
        .uri        = "/ws_harmonics",
        .method     = HTTP_GET,
        .handler    = ws_harmonics_handler,
        .is_websocket = true
    });
    httpd_register_uri_handler(server_handle, &(httpd_uri_t){
        .uri      = "/download",
        .method   = HTTP_GET,
        .handler  = download_page_handler
    });

    ESP_LOGI(TAG, "Webserver started on port %d", config.server_port);
    return server_handle;
}

esp_err_t webserver_stop(httpd_handle_t server) {
    if (!server) {
        ESP_LOGW(TAG, "Server not running");
        return ESP_ERR_INVALID_STATE;
    }
    return httpd_stop(server);
}
