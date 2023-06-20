idf_component_register(SRCS "SPI_ws2812b.c" "wifi_connect.c" "main.c" "mqtt_transmit.c" "ble_connect.c"
                    INCLUDE_DIRS "."
                    EMBED_TXTFILES "certs/AmazonRootCA1.pem"
                                   "certs/certificate.pem.crt"
                                   "certs/private.pem.key")