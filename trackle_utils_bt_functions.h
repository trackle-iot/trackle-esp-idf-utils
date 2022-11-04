#ifndef TRACKLE_UTILS_BT_FUNCTIONS_H
#define TRACKLE_UTILS_BT_FUNCTIONS_H

#define MAX_BT_FUNCTION_NAME_LEN 32
#define MAX_BT_FUNCTIONS_NUM 10

#define MAX_BT_GET_NAME_LEN 32
#define MAX_BT_GETS_NUM 10

#include <stdbool.h>

#include <esp_err.h>

#include <defines.h>

bool Trackle_BtFunction_add(const char *name, int (*function)(const char *));
bool Trackle_BtGet_add(const char *name, void (*function)(const char *), Data_TypeDef dataType);

esp_err_t btFunctionsEndpointsCreate();
esp_err_t btFunctionsEndpointsRegister();

#endif