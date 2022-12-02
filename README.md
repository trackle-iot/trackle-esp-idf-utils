# Trackle Utils Features

Complete documentation can be found at [https://trackle-iot.github.io/trackle-utils-esp-idf/](https://trackle-iot.github.io/trackle-utils-esp-idf/)

## Properties

Properties are variables whose value is published on the cloud, and that can be changed from the cloud.

See ```trackle_utils_properties.h``` for functions to be used with properties.

## Notifications

Notifications are a mechanism to tell to the cloud that something happened, along with a numeric value to give some context.

See ```trackle_utils_notifications.h``` for functions to be used with notifications.

## BT provisioning functions

BT provisioning functions are functions that can be invoked by a client (app, PC program, etc.) on the ESP32 that implements them, either to retrieve data (GET functions) or to send data (POST functions) to the device.

See ```trackle_utils_bt_functions.h``` for functions to be used with BT functions.

## Claim code

Claim code can be written to and loaded from NVS, ESP32's flash storage.

See ```trackle_utils_claimcode.h``` for functions to be used with BT functions.
