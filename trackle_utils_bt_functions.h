#ifndef TRACKLE_UTILS_BT_FUNCTIONS_H
#define TRACKLE_UTILS_BT_FUNCTIONS_H

#define MAX_BT_POST_NAME_LEN 32
#define MAX_BT_POSTS_NUM 10

#define MAX_BT_GET_NAME_LEN 32
#define MAX_BT_GETS_NUM 10

#include <stdbool.h>

#include <esp_err.h>

#include <defines.h>

/**
 * @brief Add a POST function callable during BLE provisioning (using Espressif's how provisioning mechanism).
 *
 * @param name Name that the client will use to call the function. The name must be unique between BLE POSTs and BLE GETs.
 * @param function Function to call, in the same format and with the same semantics of a Trackle cloud POST.
 * @return true The function was added successfully to POST BLE functions.
 * @return false There was and error in adding the function to POST BLE functions.
 */
bool Trackle_BtPost_add(const char *name, int (*function)(const char *));

/**
 * @brief Add a GET function callable during BLE provisioning (using Espressif's how provisioning mechanism).
 *
 * @param name Name that the client will use to call the function. The name must be unique between BLE POSTs and BLE GETs.
 * @param function Function to call, in the same format and with the same semantics of a Trackle cloud GET.
 * @param dataType One of VAR_INT, VAR_LONG, VAR_JSON, VAR_STRING, VAR_BOOLEAN, VAR_CHAR, VAR_DOUBLE. This tells the type of the variable pointed to by the pointer returned by \ref function.
 * @return true The function was added successfully to GET BLE functions.
 * @return false There was and error in adding the function to GET BLE functions.
 */
bool Trackle_BtGet_add(const char *name, void *(*function)(const char *), Data_TypeDef dataType);

/**
 * @brief ONLY FOR INTERNAL USAGE. DON'T CALL IN APPLICATION CODE!
 */
esp_err_t btFunctionsEndpointsCreate();

/**
 * @brief ONLY FOR INTERNAL USAGE. DON'T CALL IN APPLICATION CODE!
 */
esp_err_t btFunctionsEndpointsRegister();

#endif
