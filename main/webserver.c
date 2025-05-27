#include "webserver.h"
#include "adc.h"
#include "esp_http_server.h"
#include "esp_log.h"
#include <stdio.h>
#include <string.h>

static const char *TAG = "WEB";

/* ---------- kleine Hilfsfunktionen ------------------------------ */

static const char *guess_mime(const char *path)
{
    const char *dot = strrchr(path, '.');
    if (!dot) return "text/plain";
    if (strcmp(dot, ".html") == 0) return "text/html";
    if (strcmp(dot, ".css" ) == 0) return "text/css";
    if (strcmp(dot, ".js"  ) == 0) return "application/javascript";
    if (strcmp(dot, ".png" ) == 0) return "image/png";
    if (strcmp(dot, ".jpg" ) == 0) return "image/jpeg";
    if (strcmp(dot, ".gif" ) == 0) return "image/gif";
    return "text/plain";
}

static esp_err_t send_file(httpd_req_t *req, const char *path)
{
    FILE *f = fopen(path, "r");
    if (!f) {
        ESP_LOGW(TAG, "File not found: %s", path);
        return httpd_resp_send_404(req);
    }

    httpd_resp_set_type(req, guess_mime(path));

    char buf[2048];
    size_t n;
    while ((n = fread(buf, 1, sizeof(buf), f)) > 0)
        if (httpd_resp_send_chunk(req, buf, n) != ESP_OK) {
            fclose(f);
            httpd_resp_sendstr_chunk(req, NULL);  // Abbruch
            return ESP_FAIL;
        }

    fclose(f);
    return httpd_resp_send_chunk(req, NULL, 0);     // Terminator
}

/* ---------- API-Endpoint --------------------------------------- */

static esp_err_t api_adc_mode(httpd_req_t *req)
{
    char query[32] = {0}, param[8] = {0};
    if (httpd_req_get_url_query_str(req, query, sizeof(query)) == ESP_OK &&
        httpd_query_key_value(query, "mode", param, sizeof(param)) == ESP_OK)
        adc_change_mode((adc_mode_t)atoi(param));

    char resp[32];
    snprintf(resp, sizeof(resp), "{\"mode\":%d}", adc_get_mode());
    httpd_resp_set_type(req, "application/json");
    return httpd_resp_sendstr(req, resp);
}

/* ---------- statische Seiten ----------------------------------- */

static esp_err_t root_handler(httpd_req_t *req)
{   return send_file(req, "/spiffs/index.html"); }

static esp_err_t harmonics_handler(httpd_req_t *req)
{   return send_file(req, "/spiffs/harmonics.html"); }

/* ---------- Server-Start --------------------------------------- */

void webserver_start(void)
{
    httpd_config_t cfg = HTTPD_DEFAULT_CONFIG();
    httpd_handle_t srv = NULL;
    ESP_ERROR_CHECK(httpd_start(&srv, &cfg));

    /* Root-Seite */
    const httpd_uri_t root_uri = {
        .uri      = "/",
        .method   = HTTP_GET,
        .handler  = root_handler
    };
    httpd_register_uri_handler(srv, &root_uri);

    /* Harmonics-Seite */
    const httpd_uri_t harm_uri = {
        .uri      = "/harmonics.html",
        .method   = HTTP_GET,
        .handler  = harmonics_handler
    };
    httpd_register_uri_handler(srv, &harm_uri);

    /* API */
    const httpd_uri_t adc_uri = {
        .uri      = "/api/adc_mode",
        .method   = HTTP_GET,
        .handler  = api_adc_mode
    };
    httpd_register_uri_handler(srv, &adc_uri);

    ESP_LOGI(TAG, "HTTP server started");
}
