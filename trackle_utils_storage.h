#ifndef TRACKLE_UTILS_STORAGE_H
#define TRACKLE_UTILS_STORAGE_H

#include "nvs_flash.h"
#include "trackle_utils.h"

static uint8_t device_id[12];
unsigned char private_key[122];
char string_device_id[12 * 2 + 1];

#define CONFIG_PARTITION "nvs"
#define FACTORY_PARTITION "factory_data"
#define OLD_FACTORY_PARTITION "factory"

static const char *STORAGE_TAG = "storage";

nvs_handle_t config_handle;
nvs_handle_t device_handle;

int initStorage(bool has_config_partition)
{
    esp_err_t err = nvs_flash_init_partition(FACTORY_PARTITION);
    if (err == ESP_OK)
    { // FACTORY_PARTITION extists, try to read
        err = nvs_open_from_partition(FACTORY_PARTITION, "device", NVS_READONLY, &device_handle);
        if (err != ESP_OK)
        {
            return -2;
        }
        else
        {
            ESP_LOGI(STORAGE_TAG, "FACTORY_PARTITION found");
        }
    }
    else
    { // on error try with old factor partition named "factory"

        err = nvs_flash_init_partition(OLD_FACTORY_PARTITION);
        if (err == ESP_OK)
        { // OLD_FACTORY_PARTITION extists, try to read
            err = nvs_open_from_partition(OLD_FACTORY_PARTITION, "device", NVS_READONLY, &device_handle);
            if (err != ESP_OK)
            {
                return -2;
            }
            else
            {
                ESP_LOGI(STORAGE_TAG, "OLD_FACTORY_PARTITION found");
            }
        }
        else
        { // no factory or factory data partition defined
            ESP_LOGE(STORAGE_TAG, "no factory partition found");
            return -1;
        }
    }

    if (has_config_partition)
    {
        err = nvs_flash_init_partition(CONFIG_PARTITION);
        if (err != ESP_OK)
            return -3;

        err = nvs_open_from_partition(CONFIG_PARTITION, "machine", NVS_READWRITE, &config_handle);
        if (err != ESP_OK)
            return -4;
    }

    return 0;
}

esp_err_t readDeviceInfoFromStorage()
{
    size_t required_size = 12;
    esp_err_t err = nvs_get_blob(device_handle, "device_id", device_id, &required_size);
    hexToString((unsigned char *)device_id, 12, string_device_id, 25);
    required_size = 121;
    err += nvs_get_blob(device_handle, "private_key", private_key, &required_size);
    return err;
}

esp_err_t readConfigFromStorage(void *out_value, size_t out_size, const char *key)
{
    esp_err_t err = nvs_get_blob(config_handle, key, out_value, &out_size);
    ESP_LOGI(STORAGE_TAG, "reading config from nvs datastore for key %s", key);
    return err;
}

esp_err_t writeConfigToStorage(void *out_value, size_t out_size, const char *key)
{
    esp_err_t err = nvs_set_blob(config_handle, key, out_value, out_size);
    nvs_commit(config_handle);
    ESP_LOGI(STORAGE_TAG, "writing config in nvs datastore for key %s: %d bytes", key, out_size);
    return err;
}

#endif