#ifndef _MQTT_TRANSMIT_DRIVER_H_
#define _MQTT_TRANSMIT_DRIVER_H_

#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include "esp_system.h"
#include "esp_event.h"

#include "driver/gpio.h"
#include "esp_log.h"
#include "mqtt_client.h"
#include "esp_tls.h"
#include <sys/param.h>

extern const uint8_t cert_start[]               asm("_binary_AmazonRootCA1_pem_start");
extern const uint8_t cert_end[]                 asm("_binary_AmazonRootCA1_pem_end");
extern const uint8_t certificate_start[]        asm("_binary_certificate_pem_crt_start");
extern const uint8_t certificate_end[]          asm("_binary_certificate_pem_crt_end");
extern const uint8_t private_start[]            asm("_binary_private_pem_key_start");
extern const uint8_t private_end[]              asm("_binary_private_pem_key_end");

esp_mqtt_client_handle_t client;
esp_mqtt_event_handle_t event;

void mqtt_app_start(const char* URI);

#endif