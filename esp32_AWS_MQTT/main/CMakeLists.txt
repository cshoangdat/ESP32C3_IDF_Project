idf_component_register(SRCS "main.c" "wifi_connect.c" "mqtt_transmit.c" 
                    INCLUDE_DIRS "."
                    EMBED_TXTFILES "certs/AmazonRootCA1.pem"
                                   "certs/certificate.pem.crt"
                                   "certs/private.pem.key")