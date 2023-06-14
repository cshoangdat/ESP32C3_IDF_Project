#include "esp_spi_flash.h"
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
#include "esp_event.h"
#include "esp_log.h"
#include "esp_ota_ops.h"
#include "esp_http_client.h"
#include "esp_https_ota.h"
#include "wifi_connect.h"

const char* ESP_WIFI_SSID =                             "Ambi";
const char* ESP_WIFI_PASS =                             "1234ambi";
//Local Server:
//const char* URL =                                       "http://192.168.1.6:8000/esp32C3_AWS_MQTT.bin";
//Server public:
const char * URL =                          Ư            "https://raw.githubusercontent.com/cshoangdat/firmware_update/master/esp32C3_AWS_MQTT.bin";
static const char *TAG =                                "SIMPLE OTA";

extern const uint8_t certificate_pem_start[] asm("_binary_certificate_pem_start");
extern const uint8_t certificate_pem_end[] asm("_binary_certificate_pem_end");

esp_err_t _http_event_handler(esp_http_client_event_t *evt)
{
    switch (evt->event_id) {
    case HTTP_EVENT_ERROR:
        ESP_LOGI(TAG, "HTTP_EVENT_ERROR");
        break;
    case HTTP_EVENT_ON_CONNECTED:
        ESP_LOGI(TAG, "HTTP_EVENT_ON_CONNECTED");
        break;
    case HTTP_EVENT_HEADER_SENT:
        ESP_LOGI(TAG, "HTTP_EVENT_HEADER_SENT");
        break;
    case HTTP_EVENT_ON_HEADER:
        ESP_LOGI(TAG, "HTTP_EVENT_ON_HEADER, key=%s, value=%s", evt->header_key, evt->header_value);
        break;
        case HTTP_EVENT_ON_DATA:
        ESP_LOGI(TAG, "HTTP_EVENT_ON_DATA, len=%d", evt->data_len);
        break;
    case HTTP_EVENT_ON_FINISH:
        ESP_LOGI(TAG, "HTTP_EVENT_ON_FINISH");
        break;
    case HTTP_EVENT_DISCONNECTED:
        ESP_LOGI(TAG, "HTTP_EVENT_DISCONNECTED");
        break;
    }
    return ESP_OK;
}

esp_err_t do_firmware_upgrade()
{
    esp_http_client_config_t config = {
        .url = URL,
        .cert_pem = (const char*)certificate_pem_start,
        .event_handler = _http_event_handler
    };
    esp_https_ota_config_t ota_config = {
        .http_config = &config
    };
    esp_err_t ret = esp_https_ota(&config);
    if (ret == ESP_OK) {
        ESP_LOGI(TAG, "OTA Done and reboot");
        esp_restart();
    } else {
        return ret;
    }
    return ESP_OK;
}

void app_main(void)
{
    wifi_connect(ESP_WIFI_SSID, ESP_WIFI_PASS);
    vTaskDelay(2000/portTICK_PERIOD_MS);
    ESP_LOGI(TAG, "OTA Start");
    do_firmware_upgrade();
}
