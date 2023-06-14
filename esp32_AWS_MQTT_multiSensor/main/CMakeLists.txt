idf_component_register(SRCS "ltr303.c" "main.c" "wifi_connect.c" "mqtt_transmit.c" "ltr303.c" 
                    INCLUDE_DIRS "."
                    EMBED_TXTFILES "certs/AmazonRootCA1.pem"
                                   "certs/certificate.pem.crt"
                                   "certs/private.pem.key")