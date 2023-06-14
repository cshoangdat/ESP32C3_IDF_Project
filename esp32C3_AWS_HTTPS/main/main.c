#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "wifi_connect.h"
#include "http_transmit.h"
#include <esp_log.h>
#include <esp_err.h>
#include <esp_system.h>

const char* ESP_WIFI_SSID =                             "Ambi";
const char* ESP_WIFI_PASS =                             "1234ambi";
const char* URL =                                       "https://a3suuuxay09k3c-ats.iot.us-east-2.amazonaws.com:8443/topics/topic1?qos=1";
char* data = "{\"message\": \"Temp: 50.5\"}";

//curl --tlsv1.2 --cacert AmazonRootCA1.pem --cert certificate.pem.crt --key private.pem.key --request POST --data "{ \"message\": \"Lux:500\" }" "https://a3suuuxay09k3c-ats.iot.us-east-2.amazonaws.com:8443/topics/topic1?qos=1"

void app_main(void)
{
    wifi_connect(ESP_WIFI_SSID, ESP_WIFI_PASS);
    vTaskDelay(2000/portTICK_PERIOD_MS);
    client_post_rest_function(URL, data);
}
