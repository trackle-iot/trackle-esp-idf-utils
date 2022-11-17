#ifndef TRACKLE_UTILS_OTA_H
#define TRACKLE_UTILS_OTA_H

#include "esp_ota_ops.h"
#include "esp_http_client.h"
#include "esp_https_ota.h"
#include "cJSON.h"

#include "trackle_utils.h"

static const char *OTA_TAG = "trackle-utils-ota";
static const char *OTA_EVENT_NAME = "trackle/device/update/status";

char ota_url[256];
bool safe_ota = false;

/**
 * @file trackle_utils_ota.h
 * @brief Utilities to implement Over The Air firmware updates.
 */

esp_err_t _http_event_handler(esp_http_client_event_t *evt)
{
    switch (evt->event_id)
    {
    case HTTP_EVENT_ERROR:
        ESP_LOGI(OTA_TAG, "HTTP_EVENT_ERROR");
        break;
    case HTTP_EVENT_ON_CONNECTED:
        ESP_LOGI(OTA_TAG, "HTTP_EVENT_ON_CONNECTED");
        break;
    case HTTP_EVENT_HEADER_SENT:
        ESP_LOGI(OTA_TAG, "HTTP_EVENT_HEADER_SENT");
        break;
    case HTTP_EVENT_ON_HEADER:
        ESP_LOGI(OTA_TAG, "HTTP_EVENT_ON_HEADER, key=%s, value=%s", evt->header_key, evt->header_value);
        break;
    case HTTP_EVENT_ON_DATA:
        ESP_LOGI(OTA_TAG, "HTTP_EVENT_ON_DATA, len=%d", evt->data_len);
        break;
    case HTTP_EVENT_ON_FINISH:
        ESP_LOGI(OTA_TAG, "HTTP_EVENT_ON_FINISH");
        break;
    case HTTP_EVENT_DISCONNECTED:
        ESP_LOGI(OTA_TAG, "HTTP_EVENT_DISCONNECTED");
        break;
    }
    return ESP_OK;
}

void simple_ota_task(void *pvParameter)
{
    ESP_LOGW(OTA_TAG, "Starting OTA %s", ota_url);
    tracklePublishSecure(OTA_EVENT_NAME, "started");

    xEventGroupSetBits(s_wifi_event_group, OTA_UPDATING); // updating
    vTaskDelay(1000 / portTICK_PERIOD_MS);

    esp_http_client_config_t config = {
        .url = ota_url,
        .event_handler = _http_event_handler,
    };

    esp_err_t ret = esp_https_ota(&config);
    if (ret == ESP_OK)
    {
        ESP_LOGW(OTA_TAG, "OTA completed, now restarting....");
        tracklePublishSecure(OTA_EVENT_NAME, "success");
        vTaskDelay(1000 / portTICK_PERIOD_MS);
        esp_restart();
    }
    else
    {
        tracklePublishSecure(OTA_EVENT_NAME, "failed");
        xEventGroupClearBits(s_wifi_event_group, OTA_UPDATING); // stop updating
        vTaskDelete(NULL);
    }
}

/**
 * @brief Callback meant to be given as parameter to \ref trackleSetFirmwareUrlUpdateCallback to implement OTA via URL.
 *
 * @param data JSON string containing the "url" key, that points to the URL of the firmware to be downloaded.
 */
void firmware_ota_url(const char *data)
{
    ESP_LOGI(OTA_TAG, "firmware_ota_url %s", data);
    cJSON *json = cJSON_Parse(data);
    int result = -1;
    if (json != NULL)
    {
        cJSON *url = cJSON_GetObjectItemCaseSensitive(json, "url");
        if (cJSON_IsString(url) && (url->valuestring != NULL))
        {
            size_t n = strlen(url->valuestring);
            strcpy(ota_url, url->valuestring);
            ota_url[n] = '\0';
            xTaskCreate(&simple_ota_task, "simple_ota_task", 8192, NULL, 5, NULL);
            result = 1;
        }
    }
    cJSON_Delete(json);
    ESP_LOGI(OTA_TAG, "firmware_ota_url result %d", result);
}

#endif