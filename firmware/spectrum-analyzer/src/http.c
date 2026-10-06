#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "esp_log.h"
#include "esp_err.h"
#include "esp_http_server.h"
#include "config.h"
#include "fastdetect.h"  // Enthält build_chunk_json, fastdetect_handler und ws_handler
#include "wav.h"         // Enthält wav_download_handler
#include "http.h"        // Eigene Header-Datei für HTTP-Funktionen

static const char *TAG = "HTTP";

/* Favicon-Handler */
static esp_err_t favicon_handler(httpd_req_t *req)
{
    httpd_resp_set_status(req, "204 No Content");
    return httpd_resp_send(req, NULL, 0);
}

/* Index-Handler: Liefert /spiffs/index.html */
static esp_err_t index_handler(httpd_req_t *req)
{
    FILE *f = fopen("/spiffs/index.html", "r");
    if (!f) {
        ESP_LOGE(TAG, "Failed to open /spiffs/index.html");
        httpd_resp_send_err(req, HTTPD_404_NOT_FOUND, "HTML file not found");
        return ESP_FAIL;
    }
    fseek(f, 0, SEEK_END);
    long size = ftell(f);
    fseek(f, 0, SEEK_SET);
    if (size < 0) {
        fclose(f);
        ESP_LOGE(TAG, "Failed to determine size of /spiffs/index.html");
        httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "ftell failed");
        return ESP_FAIL;
    }
    char *buf = (char*)malloc(size + 1);
    if (!buf) {
        fclose(f);
        ESP_LOGE(TAG, "Failed to allocate memory for index.html");
        httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "OOM");
        return ESP_FAIL;
    }
    fread(buf, 1, size, f);
    fclose(f);
    buf[size] = '\0';
    httpd_resp_set_type(req, "text/html");
    esp_err_t res = httpd_resp_send(req, buf, size);
    free(buf);
    return res;
}

/* Fastdetect-Handler: Liefert /spiffs/fastdetect.html */
esp_err_t fastdetect_handler(httpd_req_t *req)
{
    FILE *f = fopen("/spiffs/fastdetect.html", "r");
    if (!f) {
        ESP_LOGE(TAG, "Failed to open /spiffs/fastdetect.html");
        httpd_resp_send_err(req, HTTPD_404_NOT_FOUND, "HTML file not found");
        return ESP_FAIL;
    }
    fseek(f, 0, SEEK_END);
    long size = ftell(f);
    fseek(f, 0, SEEK_SET);
    if (size < 0) {
        fclose(f);
        ESP_LOGE(TAG, "Failed to determine size of /spiffs/fastdetect.html");
        httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "ftell failed");
        return ESP_FAIL;
    }
    char *buf = (char*)malloc(size + 1);
    if (!buf) {
        fclose(f);
        ESP_LOGE(TAG, "Failed to allocate memory for fastdetect.html");
        httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "OOM");
        return ESP_FAIL;
    }
    fread(buf, 1, size, f);
    fclose(f);
    buf[size] = '\0';
    httpd_resp_set_type(req, "text/html");
    esp_err_t res = httpd_resp_send(req, buf, size);
    free(buf);
    return res;
}

/* Monitoring-Handler: Liefert /spiffs/monitoring.html */
esp_err_t monitoring_handler(httpd_req_t *req)
{
    FILE *f = fopen("/spiffs/monitoring.html", "r");
    if (!f) {
        ESP_LOGE(TAG, "Failed to open /spiffs/monitoring.html");
        httpd_resp_send_err(req, HTTPD_404_NOT_FOUND, "HTML file not found");
        return ESP_FAIL;
    }
    fseek(f, 0, SEEK_END);
    long size = ftell(f);
    fseek(f, 0, SEEK_SET);
    if (size < 0) {
        fclose(f);
        ESP_LOGE(TAG, "Failed to determine size of /spiffs/monitoring.html");
        httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "ftell failed");
        return ESP_FAIL;
    }
    char *buf = (char*)malloc(size + 1);
    if (!buf) {
        fclose(f);
        ESP_LOGE(TAG, "Failed to allocate memory for monitoring.html");
        httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "OOM");
        return ESP_FAIL;
    }
    fread(buf, 1, size, f);
    fclose(f);
    buf[size] = '\0';
    httpd_resp_set_type(req, "text/html");
    esp_err_t res = httpd_resp_send(req, buf, size);
    free(buf);
    return res;
}

/* WAV-Handler: Liefert den generierten WAV-Stream (via wav_download_handler aus wav.c) */
esp_err_t wav_handler(httpd_req_t *req)
{
    return wav_download_handler(req);
}

/* WebSocket-Handler */
esp_err_t ws_handler(httpd_req_t *req)
{
    if (req->method == HTTP_GET) {
        ESP_LOGI(TAG, "New WS client connected");
        return ESP_OK;
    }
    httpd_ws_frame_t ws_pkt;
    memset(&ws_pkt, 0, sizeof(ws_pkt));
    ws_pkt.type = HTTPD_WS_TYPE_TEXT;
    esp_err_t ret = httpd_ws_recv_frame(req, &ws_pkt, 0);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "ws_handler: get length error: %d", ret);
        return ret;
    }
    if (ws_pkt.len) {
        uint8_t *buf = (uint8_t*)calloc(1, ws_pkt.len + 1);
        if (!buf)
            return ESP_ERR_NO_MEM;
        ws_pkt.payload = buf;
        ret = httpd_ws_recv_frame(req, &ws_pkt, ws_pkt.len);
        if (ret == ESP_OK) {
            ESP_LOGI(TAG, "WS got: %s", ws_pkt.payload);
            if (strcmp((char*)ws_pkt.payload, "getdata") == 0) {
                char json[JSON_BUFFER_SIZE];
                memset(json, 0, sizeof(json));
                build_chunk_json(json, sizeof(json));
                ESP_LOGI(TAG, "Sending JSON: %s", json);
                httpd_ws_frame_t resp;
                memset(&resp, 0, sizeof(resp));
                resp.type = HTTPD_WS_TYPE_TEXT;
                resp.payload = (uint8_t*)json;
                resp.len = strlen(json);
                esp_err_t r2 = httpd_ws_send_frame(req, &resp);
                if (r2 != ESP_OK) {
                    ESP_LOGW(TAG, "Send chunk JSON failed: %d", r2);
                }
            }
        } else {
            ESP_LOGE(TAG, "ws_handler: data recv error: %d", ret);
        }
        free(buf);
    }
    return ESP_OK;
}

/* Startet den HTTP-Server und registriert alle Handler */
httpd_handle_t start_webserver(void)
{
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    httpd_handle_t server = NULL;
    if (httpd_start(&server, &config) == ESP_OK) {
        // /favicon.ico
        httpd_uri_t favicon_uri = {
            .uri = "/favicon.ico",
            .method = HTTP_GET,
            .handler = favicon_handler,
            .user_ctx = NULL
        };
        httpd_register_uri_handler(server, &favicon_uri);
        // /fastdetect
        httpd_uri_t fastdetect_uri = {
            .uri = "/fastdetect",
            .method = HTTP_GET,
            .handler = fastdetect_handler,
            .user_ctx = NULL
        };
        httpd_register_uri_handler(server, &fastdetect_uri);
        // /monitoring
        httpd_uri_t monitoring_uri = {
            .uri = "/monitoring",
            .method = HTTP_GET,
            .handler = monitoring_handler,
            .user_ctx = NULL
        };
        httpd_register_uri_handler(server, &monitoring_uri);
        // /wav
        httpd_uri_t wav_uri = {
            .uri = "/wav",
            .method = HTTP_GET,
            .handler = wav_handler,
            .user_ctx = NULL
        };
        httpd_register_uri_handler(server, &wav_uri);
        // /ws (WebSocket)
        httpd_uri_t ws_uri = {
            .uri = "/ws",
            .method = HTTP_GET,
            .handler = ws_handler,
            .user_ctx = NULL,
            .is_websocket = true
        };
        httpd_register_uri_handler(server, &ws_uri);
        // / (index.html)
        httpd_uri_t index_uri = {
            .uri = "/",
            .method = HTTP_GET,
            .handler = index_handler,
            .user_ctx = NULL
        };
        httpd_register_uri_handler(server, &index_uri);
        ESP_LOGI(TAG, "Webserver started on port %d", config.server_port);
    } else {
        ESP_LOGE(TAG, "Error starting HTTP server!");
    }
    return server;
}
