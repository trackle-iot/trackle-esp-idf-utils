#ifndef TRACKLE_UTILS_BT_PROVISION_H
#define TRACKLE_UTILS_BT_PROVISION_H

#include "nvs_flash.h"

#include "trackle_utils_wifi.h"
#include "trackle_utils.h"

#include <wifi_provisioning/manager.h>
#include <wifi_provisioning/scheme_ble.h>

#define PROV_MGR_MAX_RETRY_CNT 2
int prov_retry_num = 0;

static const char *BT_TAG = "trackle-utils-bt-provision";

/* Event handler for catching system events */
static void bt_event_handler(void *arg, esp_event_base_t event_base,
                             int32_t event_id, void *event_data)
{
    ESP_LOGI(BT_TAG, "----------------------------------------");
    ESP_LOGI(BT_TAG, "bt event_handler: %s %d", event_base, event_id);
    ESP_LOGI(BT_TAG, "----------------------------------------");

    if (event_base == WIFI_PROV_EVENT)
    {
        switch (event_id)
        {
        case WIFI_PROV_START:
            ESP_LOGI(BT_TAG, "Provisioning started");
            break;
        case WIFI_PROV_CRED_RECV:
        {
            wifi_sta_config_t *wifi_sta_cfg = (wifi_sta_config_t *)event_data;
            ESP_LOGI(BT_TAG, "Received Wi-Fi credentials"
                             "\n\tSSID     : %s\n\tPassword : %s",
                     (const char *)wifi_sta_cfg->ssid,
                     (const char *)wifi_sta_cfg->password);
            break;
        }
        case WIFI_PROV_CRED_FAIL:
        {
            wifi_prov_sta_fail_reason_t *reason = (wifi_prov_sta_fail_reason_t *)event_data;
            ESP_LOGE(BT_TAG, "Provisioning failed!\n\tReason : %s"
                             "\n\tPlease reset to factory and retry provisioning",
                     (*reason == WIFI_PROV_STA_AUTH_ERROR) ? "Wi-Fi station authentication failed" : "Wi-Fi access-point not found");

            prov_retry_num++;
            if (prov_retry_num >= PROV_MGR_MAX_RETRY_CNT)
            {
                ESP_LOGI(BT_TAG, "Failed to connect with provisioned AP, reseting provisioned credentials and restarting...");
                wifi_config_t wifi_cfg = {0};
                esp_err_t err = esp_wifi_set_config(WIFI_IF_STA, &wifi_cfg);
                if (err != ESP_OK)
                {
                    ESP_LOGE(BT_TAG, "Failed to set wifi config, 0x%x", err);
                }

                xEventGroupSetBits(s_wifi_event_group, RESTART);
            }

            break;
        }
        case WIFI_PROV_CRED_SUCCESS:
            ESP_LOGI(BT_TAG, "Provisioning successful");
            break;
        case WIFI_PROV_END:
            // De-initialize manager once provisioning is finished and restart
            ESP_LOGI(BT_TAG, "Provisioning end");
            wifi_prov_mgr_deinit();
            xEventGroupSetBits(s_wifi_event_group, RESTART);
            break;
        default:
            break;
        }
    }

    ESP_LOGI(BT_TAG, "end bt_event_handler: -------------------");
}

static void get_device_service_name(char *service_name, size_t max)
{
    uint8_t eth_mac[6];
    const char *ssid_prefix = "DEVICE_";
    esp_wifi_get_mac(WIFI_IF_STA, eth_mac);
    snprintf(service_name, max, "%s%02X%02X%02X", ssid_prefix, eth_mac[3], eth_mac[4], eth_mac[5]);
}

void trackle_utils_bt_provision_init()
{
    ESP_ERROR_CHECK(esp_event_handler_register(WIFI_PROV_EVENT, ESP_EVENT_ANY_ID, &bt_event_handler, NULL));
}

void trackle_utils_bt_provision_loop()
{
    EventBits_t bits = xEventGroupGetBits(s_wifi_event_group);

    // Check if start or stop provisioning
    if (bits & START_PROVISIONING)
    {
        xEventGroupClearBits(s_wifi_event_group, START_PROVISIONING);
        xEventGroupSetBits(s_wifi_event_group, IS_PROVISIONING);

        esp_wifi_set_ps(WIFI_PS_MIN_MODEM); // enable powersave

        // Configuration for the provisioning manager
        wifi_prov_mgr_config_t config = {
            .scheme = wifi_prov_scheme_ble,
            .scheme_event_handler = WIFI_PROV_SCHEME_BLE_EVENT_HANDLER_FREE_BTDM,
        };

        // Initialize provisioning manager
        wifi_prov_mgr_init(config);

        char service_name[14];
        get_device_service_name(service_name, sizeof(service_name));

        wifi_prov_security_t security = WIFI_PROV_SECURITY_1;

        const char *service_key = NULL;
        uint8_t custom_service_uuid[] = {0xb4, 0xdf, 0x5a, 0x1c, 0x3f, 0x6b, 0xf4, 0xbf, 0xea, 0x4a, 0x82, 0x03, 0x04, 0x90, 0x1a, 0x02};
        wifi_prov_scheme_ble_set_service_uuid(custom_service_uuid);

        esp_err_t prov_err = wifi_prov_mgr_start_provisioning(security, NULL, service_name, service_key);
        ESP_LOGI(BT_TAG, "wifi_prov_mgr_start_provisioning %d", prov_err);
    }
}

#endif