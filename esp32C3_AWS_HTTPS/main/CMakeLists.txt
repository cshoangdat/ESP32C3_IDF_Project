# Edit following two lines to set component requirements (see docs)
idf_component_register(SRCS "main.c" "wifi_connect.c" "http_transmit.c"
                    INCLUDE_DIRS "."
                    EMBED_TXTFILES "certs/AmazonRootCA1.pem"
                                   "certs/certificate.pem.crt"
                                   "certs/private.pem.key")
