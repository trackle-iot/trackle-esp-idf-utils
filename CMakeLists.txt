idf_component_register(SRCS "./src/trackle_utils.c" "./src/trackle_utils_notifications.c" "./src/trackle_utils_properties.c" "./src/trackle_utils_bt_functions.c"
                       INCLUDE_DIRS "."
                       REQUIRES trackle-esp-idf wifi_provisioning)
