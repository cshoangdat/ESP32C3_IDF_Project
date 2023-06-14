idf_component_register(SRCS "main.c" "wifi_connect.c" "http_transmit.c" "ltr303.c"
                    INCLUDE_DIRS "."
                    EMBED_TXTFILES certificate.pem)