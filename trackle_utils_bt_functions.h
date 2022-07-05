#ifndef TRACKLE_UTILS_BT_FUNCTIONS_H
#define TRACKLE_UTILS_BT_FUNCTIONS_H

#define MAX_BT_FUNCTION_NAME_LEN 32
#define MAX_BT_FUNCTIONS_NUM 10

#include <stdbool.h>

#include <esp_err.h>

bool Trackle_BtFunction_add(const char *name, int (*function)(const char *));

esp_err_t btFunctionsEndpointsCreate();
esp_err_t btFunctionsEndpointsRegister();

#endif