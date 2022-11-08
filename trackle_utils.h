#ifndef TRACKLE_UTILS_H
#define TRACKLE_UTILS_H

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"

#include <sys/time.h>
#include <stdio.h>
#include <string.h>

#define TRACKLE_UTILS_VERSION "2.0.0"

// commons bits definitions
extern EventGroupHandle_t s_wifi_event_group;

#define WIFI_CONNECTED_BIT BIT0
#define WIFI_TO_CONNECT_BIT BIT1

#define START_PROVISIONING BIT2
#define IS_PROVISIONING BIT3

#define RESTART BIT4
#define OTA_UPDATING BIT5

// metodo che ritorno i ms passati dall'avvio del firmware
static uint32_t millis(void)
{
    struct timeval tp;
    gettimeofday(&tp, NULL);
    return (uint32_t)(tp.tv_sec * 1000 + tp.tv_usec / 1000);
}

void hexToString(unsigned char *in, size_t insz, char *out, size_t outz);
int stringToHex(char *hex_str, unsigned char *byte_array, int byte_array_max);
int splitString(char *value, const char *separator, char *results[], size_t max_results);
bool isValid(char *input, const char *op, int b);
time_t getGmTimestamp();

#endif
