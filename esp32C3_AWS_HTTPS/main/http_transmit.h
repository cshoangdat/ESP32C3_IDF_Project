#ifndef _HTTP_DRIVER_H_
#define _HTTP_DRIVER_H_

#include <stdio.h>
#include <sys/param.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_wifi.h"
#include "esp_log.h"
#include "esp_http_client.h"
#include <esp_event.h>
#include <esp_system.h>
#include "esp_tls.h"

static const char *TAG = "HTTPS";

//Define client certificate 
extern const uint8_t cert_start[] asm("_binary_AmazonRootCA1_pem_start");
extern const uint8_t cert_end[] asm("_binary_AmazonRootCA1_pem_end");
extern const uint8_t certificate_start[] asm("_binary_certificate_pem_crt_start");
extern const uint8_t certificate_end[] asm("_binary_certificate_pem_crt_end");
extern const uint8_t private_start[] asm("_binary_private_pem_key_start");
extern const uint8_t private_end[] asm("_binary_private_pem_key_end");


void client_post_rest_function(const char *url, char* data);
void client_get_rest_function(const char *url);

#endif