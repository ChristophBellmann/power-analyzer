#include "webserver.h"
#include "wifi.h"
#include "adc.h"
#include "fft.h"
#include "esp_http_server.h"
#include "esp_log.h"
#include <stdlib.h>
#include <string.h>

static const char *TAG = "WEB";

/* Forward declarations */
static esp_err_t index_handler(httpd_req_t *req);
static esp_err_t harm_page_handler(httpd_req_t *req);
static esp_err_t harmonics_handler(httpd_req_t *req);
static esp_err_t download_page_handler(httpd_req_t *req);

/* ------------------------------------------------------------ */
/* /api/adc_mode                                                */
/* ------------------------------------------------------------ */
static esp_err_t api_adc_mode(httpd_req_t *req)
{
    char query[32] = {0};
    if (httpd_req_get_url_query_str(req, query, sizeof(query)) == ESP_OK) {
        char param[8] = {0};
        if (httpd_query_key_value(query, "mode", param, sizeof(param)) == ESP_OK) {
            int mode = atoi(param);
            adc_change_mode((adc_mode_t)mode);
        }
    }

    int current = adc_get_mode();
    char resp[32];
    snprintf(resp, sizeof(resp), "{\"mode\":%d}", current);
    httpd_resp_set_type(req, "application/json");
    return httpd_resp_sendstr(req, resp);
}

void webserver_start(void)
{
    httpd_config_t cfg = HTTPD_DEFAULT_CONFIG();
    httpd_handle_t srv = NULL;
    ESP_ERROR_CHECK(httpd_start(&srv, &cfg));

    const httpd_uri_t root_uri = {
        .uri      = "/",
        .method   = HTTP_GET,
        .handler  = index_handler,
        .user_ctx = NULL
    };
    ESP_ERROR_CHECK(httpd_register_uri_handler(srv, &root_uri));

    const httpd_uri_t harm_page_uri = {
        .uri      = "/harmonics.html",
        .method   = HTTP_GET,
        .handler  = harm_page_handler,
        .user_ctx = NULL
    };
    ESP_ERROR_CHECK(httpd_register_uri_handler(srv, &harm_page_uri));

    const httpd_uri_t harm_json_uri = {
        .uri      = "/harmonics",
        .method   = HTTP_GET,
        .handler  = harmonics_handler,
        .user_ctx = NULL
    };
    ESP_ERROR_CHECK(httpd_register_uri_handler(srv, &harm_json_uri));

    const httpd_uri_t down_uri = {
        .uri      = "/download",
        .method   = HTTP_GET,
        .handler  = download_page_handler,
        .user_ctx = NULL
    };
    ESP_ERROR_CHECK(httpd_register_uri_handler(srv, &down_uri));

    const httpd_uri_t adc_uri = {
        .uri      = "/api/adc_mode",
        .method   = HTTP_GET,
        .handler  = api_adc_mode,
        .user_ctx = NULL
    };
    ESP_ERROR_CHECK(httpd_register_uri_handler(srv, &adc_uri));

    ESP_LOGI(TAG, "HTTP server started");
}

/* ------------------------------------------------------------ */
/* Simple placeholder handlers                                  */
/* ------------------------------------------------------------ */
static esp_err_t index_handler(httpd_req_t *req)
{
    const char *resp = "<html><body><h1>Power Analyzer</h1></body></html>";
    return httpd_resp_send(req, resp, HTTPD_RESP_USE_STRLEN);
}

static esp_err_t harm_page_handler(httpd_req_t *req)
{
    const char *resp = "<html><body><h1>Harmonics Page</h1></body></html>";
    return httpd_resp_send(req, resp, HTTPD_RESP_USE_STRLEN);
}

static esp_err_t harmonics_handler(httpd_req_t *req)
{
    const char *resp = "{\"status\":\"ok\"}";
    httpd_resp_set_type(req, "application/json");
    return httpd_resp_send(req, resp, HTTPD_RESP_USE_STRLEN);
}

static esp_err_t download_page_handler(httpd_req_t *req)
{
    const char *resp = "<html><body><h1>Download</h1></body></html>";
    return httpd_resp_send(req, resp, HTTPD_RESP_USE_STRLEN);
}
