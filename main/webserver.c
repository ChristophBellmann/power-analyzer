// main/webserver.c

#include "webserver.h"
#include "config.h"
#include "config_manager.h"
#include "recorder.h"        // download_handler(), send_scope_frame()
#include "adc.h"             // adc_get_mode(), adc_change_mode()
#include "harmonics.h"       // harmonics_handler(), ws_harmonics_handler()
#include "esp_http_server.h"
#include "esp_log.h"
#include <stdio.h>
#include <string.h>

static const char *TAG = "WEB";

/* --------------------------------------
 * Guess MIME type by file extension
 * ------------------------------------- */
static const char *guess_mime(const char *path)
{
    const char *dot = strrchr(path, '.');
    if (!dot) return "text/plain";
    if (strcmp(dot, ".html") == 0)    return "text/html";
    if (strcmp(dot, ".css" ) == 0)    return "text/css";
    if (strcmp(dot, ".js"  ) == 0)    return "application/javascript";
    if (strcmp(dot, ".png" ) == 0)    return "image/png";
    if (strcmp(dot, ".jpg" ) == 0)    return "image/jpeg";
    if (strcmp(dot, ".gif" ) == 0)    return "image/gif";
    return "text/plain";
}

/* ------------------------------------------------
 * Send a static file from SPIFFS (/spiffs/)
 * ------------------------------------------------ */
static esp_err_t send_file(httpd_req_t *req, const char *path)
{
    FILE *f = fopen(path, "r");
    if (!f) {
        ESP_LOGW(TAG, "File not found: %s", path);
        return httpd_resp_send_err(req, HTTPD_404_NOT_FOUND, "Not found");
    }
    httpd_resp_set_type(req, guess_mime(path));

    char buf[1024];
    size_t len;
    while ((len = fread(buf, 1, sizeof(buf), f)) > 0) {
        if (httpd_resp_send_chunk(req, buf, len) != ESP_OK) {
            fclose(f);
            httpd_resp_send_chunk(req, NULL, 0);
            return ESP_FAIL;
        }
    }
    fclose(f);
    return httpd_resp_send_chunk(req, NULL, 0);
}

/* --------------------------
 * GET /api/config
 * ------------------------- */
static esp_err_t api_config_get(httpd_req_t *req)
{
    char json[128];
    int len = snprintf(json, sizeof(json),
        "{"
          "\"traceWidth\":%d,"
          "\"gridWidth\":%d,"
          "\"traceVColor\":\"%s\","
          "\"traceIColor\":\"%s\""
        "}",
        config_get_trace_width(),
        config_get_grid_width(),
        config_get_trace_v_color(),
        config_get_trace_i_color()
    );
    httpd_resp_set_type(req, "application/json");
    return httpd_resp_send(req, json, len);
}

/* --------------------------
 * POST /api/config
 * ------------------------- */
static esp_err_t api_config_post(httpd_req_t *req)
{
    int total_len = req->content_len;
    if (total_len <= 0 || total_len > 256) {
        return httpd_resp_send_err(req, HTTPD_400_BAD_REQUEST, "Bad Request");
    }

    char buf[256];
    int ret = httpd_req_recv(req, buf, total_len);
    if (ret <= 0) {
        return httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "Receive failed");
    }
    buf[ret] = '\0';

    int tw, gw;
    char vc[16], ic[16];
    int matched = sscanf(buf,
        "{\"traceWidth\":%d,"
         "\"gridWidth\":%d,"
         "\"traceVColor\":\"%15[^\"]\","
         "\"traceIColor\":\"%15[^\"]\"}",
        &tw, &gw, vc, ic
    );
    if (matched != 4) {
        return httpd_resp_send_err(req, HTTPD_400_BAD_REQUEST, "Bad JSON");
    }

    config_set_trace_width(tw);
    config_set_grid_width(gw);
    config_set_trace_v_color(vc);
    config_set_trace_i_color(ic);
    if (config_manager_save() != ESP_OK) {
        return httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "Save failed");
    }

    return httpd_resp_send(req, NULL, 0);
}

/* --------------------------
 * GET /api/adc_mode
 * ------------------------- */
static esp_err_t api_adc_mode(httpd_req_t *req)
{
    char query[32] = {0}, param[8] = {0};
    if (httpd_req_get_url_query_str(req, query, sizeof(query)) == ESP_OK &&
        httpd_query_key_value(query, "mode", param, sizeof(param)) == ESP_OK)
    {
        adc_change_mode((adc_mode_t)atoi(param));
    }

    char resp[32];
    int len = snprintf(resp, sizeof(resp), "{\"mode\":%d}", adc_get_mode());
    httpd_resp_set_type(req, "application/json");
    return httpd_resp_send(req, resp, len);
}

/* --------------------------
 * GET /download
 * ------------------------- */
static esp_err_t download_handler_wrapper(httpd_req_t *req)
{
    return download_handler(req);
}

/* --------------------------
 * WebSocket /ws_scope
 * ------------------------- */
static esp_err_t ws_scope_handler(httpd_req_t *req)
{
    if (req->method == HTTP_GET) {
        ESP_LOGI(TAG, "WS_SCOPE handshake");
        return ESP_OK;
    }
    httpd_ws_frame_t in_frame = { .type = HTTPD_WS_TYPE_TEXT };
    esp_err_t err = httpd_ws_recv_frame(req, &in_frame, 0);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "WS_SCOPE recv err %s", esp_err_to_name(err));
        return err;
    }
    return send_scope_frame(req);
}

/* --------------------------
 * WebSocket /ws_harmonics
 * ------------------------- */
static esp_err_t ws_harmonics_wrapper(httpd_req_t *req)
{
    return ws_harmonics_handler(req);
}

/* --------------------------
 * Static pages handlers
 * ------------------------- */
static esp_err_t root_handler(httpd_req_t *req)
{
    return send_file(req, "/spiffs/index.html");
}

static esp_err_t harm_page_handler(httpd_req_t *req)
{
    return send_file(req, "/spiffs/harmonics.html");
}

/* ---------------------------------------
 * Launch the HTTP & WebSocket server
 * -------------------------------------- */
void webserver_start(void)
{
    httpd_config_t cfg = HTTPD_DEFAULT_CONFIG();
    cfg.stack_size = 8192;  // allocate extra stack for handlers
    httpd_handle_t server = NULL;
    ESP_ERROR_CHECK(httpd_start(&server, &cfg));

    // 1) Static content
    httpd_register_uri_handler(server, &(httpd_uri_t){
        .uri      = "/",
        .method   = HTTP_GET,
        .handler  = root_handler
    });
    httpd_register_uri_handler(server, &(httpd_uri_t){
        .uri      = "/harmonics.html",
        .method   = HTTP_GET,
        .handler  = harm_page_handler
    });

    // 2) CSV download
    httpd_register_uri_handler(server, &(httpd_uri_t){
        .uri      = "/download",
        .method   = HTTP_GET,
        .handler  = download_handler_wrapper
    });

    // 3) ADC mode API
    httpd_register_uri_handler(server, &(httpd_uri_t){
        .uri      = "/api/adc_mode",
        .method   = HTTP_GET,
        .handler  = api_adc_mode
    });

    // 4) Config API
    httpd_register_uri_handler(server, &(httpd_uri_t){
        .uri      = "/api/config",
        .method   = HTTP_GET,
        .handler  = api_config_get
    });
    httpd_register_uri_handler(server, &(httpd_uri_t){
        .uri      = "/api/config",
        .method   = HTTP_POST,
        .handler  = api_config_post
    });

    // 5) WebSockets
    httpd_register_uri_handler(server, &(httpd_uri_t){
        .uri          = "/ws_scope",
        .method       = HTTP_GET,
        .handler      = ws_scope_handler,
        .is_websocket = true
    });
    httpd_register_uri_handler(server, &(httpd_uri_t){
        .uri          = "/ws_harmonics",
        .method       = HTTP_GET,
        .handler      = ws_harmonics_wrapper,
        .is_websocket = true
    });

    ESP_LOGI(TAG, "HTTP server started");
}
