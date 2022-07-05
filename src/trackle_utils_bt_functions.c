#include "trackle_utils_bt_functions.h"

#include <wifi_provisioning/manager.h>

typedef struct
{
    char name[MAX_BT_FUNCTION_NAME_LEN];
    int (*function)(const char *arg);
} BtFunction_t;

static int actualBtFunctionsNum = 0;
static BtFunction_t btFunctions[MAX_BT_FUNCTIONS_NUM];

bool Trackle_BtFunction_add(const char *name, int (*function)(const char *))
{
    if (actualBtFunctionsNum < MAX_BT_FUNCTIONS_NUM)
    {
        for (int i = 0; i < actualBtFunctionsNum; i++)
        {
            if (strcmp(btFunctions[i].name, name) == 0)
            {
                return false; // Fail, functions already exist
            }
        }
        if (strlen(name) + 1 > MAX_BT_FUNCTION_NAME_LEN) // +1 because there must be space for null character
        {
            return false;
        }
        strcpy(btFunctions[actualBtFunctionsNum].name, name);
        btFunctions[actualBtFunctionsNum].function = function;
        actualBtFunctionsNum++;
        return true;
    }
    return false;
}

static esp_err_t btFunctionCallHandler(uint32_t session_id, const uint8_t *inbuf, ssize_t inlen, uint8_t **outbuf, ssize_t *outlen, void *priv_data)
{
    const int btFunIdx = *((int *)&priv_data); // Interpret pointer as an integer representing the index of the function of interest.
    int (*function)(const char *) = btFunctions[btFunIdx].function;
    const int funRes = function((const char *)inbuf); // TODO: return funRes in some way to the user (outbuf?).
    return ESP_OK;
}

esp_err_t btFunctionsEndpointsCreate()
{
    for (int i = 0; i < actualBtFunctionsNum; i++)
    {
        const esp_err_t err = wifi_prov_mgr_endpoint_create(btFunctions[i].name);
        if (err != ESP_OK)
        {
            return err;
        }
    }
    return ESP_OK;
}

esp_err_t btFunctionsEndpointsRegister()
{
    for (int i = 0; i < actualBtFunctionsNum; i++)
    {
        const esp_err_t err = wifi_prov_mgr_endpoint_register(btFunctions[i].name, btFunctionCallHandler, *((void **)&i));
        if (err != ESP_OK)
        {
            return err;
        }
    }
    return ESP_OK;
}