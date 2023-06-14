#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include "esp_system.h"
#include "nvs_flash.h"
#include "esp_event.h"
#include "esp_netif.h"

#include "esp_log.h"
#include "mqtt_client.h"
#include "esp_tls.h"
#include "esp_ota_ops.h"
#include <sys/param.h>
#include "driver/gpio.h"

extern const uint8_t cert_start[] asm("_binary_AmazonRootCA1_pem_start");
extern const uint8_t cert_end[] asm("_binary_AmazonRootCA1_pem_end");
extern const uint8_t certificate_start[] asm("_binary_certificate_pem_crt_start");
extern const uint8_t certificate_end[] asm("_binary_certificate_pem_crt_end");
extern const uint8_t private_start[] asm("_binary_private_pem_key_start");
extern const uint8_t private_end[] asm("_binary_private_pem_key_end");

esp_mqtt_client_handle_t client;
esp_mqtt_event_handle_t event;

// #include "mqtt_transmit.h"
#include "wifi_connect.h"

const char* ESP_WIFI_SSID =                             "Ambi";
const char* ESP_WIFI_PASS =                             "1234ambi";
const char* URI =                                       "mqtts://a3suuuxay09k3c-ats.iot.us-east-2.amazonaws.com";
const char* TOPIC =                                     "Data";
char data[50];
static const char *TAG = "AWS_MQTT";
//curl --tlsv1.2 --cacert AmazonRootCA1.pem --cert certificate.pem.crt --key private.pem.key --request POST --data "{ \"message\": \"Lux:500\" }" "https://a3suuuxay09k3c-ats.iot.us-east-2.amazonaws.com:8443/topics/topic1?qos=1"

static void mqtt_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data)
{
    ESP_LOGD(TAG, "Event dispatched from event loop base=%s, event_id=%d", base, event_id);
    event = event_data;
    client = event->client;
    int msg_id;
    switch ((esp_mqtt_event_id_t)event_id) {
    case MQTT_EVENT_CONNECTED:
        ESP_LOGI(TAG, "MQTT_EVENT_CONNECTED");
        esp_mqtt_client_subscribe(client, TOPIC, 0);
        break;
    case MQTT_EVENT_DISCONNECTED:
        ESP_LOGI(TAG, "MQTT_EVENT_DISCONNECTED");
        break;
    case MQTT_EVENT_SUBSCRIBED:
        ESP_LOGI(TAG, "MQTT_EVENT_SUBSCRIBED, msg_id=%d", event->msg_id);
        break;
    case MQTT_EVENT_UNSUBSCRIBED:
        ESP_LOGI(TAG, "MQTT_EVENT_UNSUBSCRIBED, msg_id=%d", event->msg_id);
        break;
    case MQTT_EVENT_PUBLISHED:
        ESP_LOGI(TAG, "MQTT_EVENT_PUBLISHED, msg_id=%d", event->msg_id);
        break;
    case MQTT_EVENT_DATA:
        ESP_LOGI(TAG, "MQTT_EVENT_DATA");
        printf("TOPIC=%.*s\r\n", event->topic_len, event->topic);
        printf("DATA=%.*s\r\n", event->data_len, event->data);
        if (strncmp(event->data, "LED ON", event->data_len) == 0) {
            ESP_LOGI(TAG, "TURN LED ON");
            gpio_set_direction(GPIO_NUM_2, GPIO_MODE_OUTPUT);
            gpio_set_level(GPIO_NUM_2, 1);
        }
        if(strncmp(event->data, "LED OFF", event->data_len) == 0){
            ESP_LOGI(TAG, "TURN LED OFF");
            gpio_set_direction(GPIO_NUM_2, GPIO_MODE_OUTPUT);
            gpio_set_level(GPIO_NUM_2, 0);
        }
        break;
    case MQTT_EVENT_ERROR:
        ESP_LOGI(TAG, "MQTT_EVENT_ERROR");
        if (event->error_handle->error_type == MQTT_ERROR_TYPE_TCP_TRANSPORT) {
            ESP_LOGI(TAG, "Last error code reported from esp-tls: 0x%x", event->error_handle->esp_tls_last_esp_err);
            ESP_LOGI(TAG, "Last tls stack error number: 0x%x", event->error_handle->esp_tls_stack_err);
            ESP_LOGI(TAG, "Last captured errno : %d (%s)",  event->error_handle->esp_transport_sock_errno,
            strerror(event->error_handle->esp_transport_sock_errno));
        } else if (event->error_handle->error_type == MQTT_ERROR_TYPE_CONNECTION_REFUSED) {
            ESP_LOGI(TAG, "Connection refused error: 0x%x", event->error_handle->connect_return_code);
        } else {
            ESP_LOGW(TAG, "Unknown error type: 0x%x", event->error_handle->error_type);
        }
        break;
    default:
        ESP_LOGI(TAG, "Other event id:%d", event->event_id);
        break;
    }
}

static void mqtt_app_start(const char* URI)
{
    const esp_mqtt_client_config_t mqtt_cfg = {
        .uri = URI,
        .port = 8883,
        .cert_pem = (const char*) cert_start,
        .client_cert_pem = (const char*) certificate_start,
        .client_key_pem = (const char*) private_start
    };
    client = esp_mqtt_client_init(&mqtt_cfg);
    esp_mqtt_client_register_event(client, ESP_EVENT_ANY_ID, mqtt_event_handler, NULL);
    esp_mqtt_client_start(client);
}

void app_main(void)
{
    wifi_connect(ESP_WIFI_SSID, ESP_WIFI_PASS);
    vTaskDelay(2000/portTICK_PERIOD_MS);
    mqtt_app_start(URI);
    // esp_mqtt_client_subscribe(client, TOPIC, 0);
    for(int i = 0; i < 10 ; i++){
        sprintf(data,"{\"Temp\": \"%d\"}",i );
        int msg_id = esp_mqtt_client_publish(client, "Data", data, 0, 0, 0);
        ESP_LOGI(TAG, "sent publish successful, msg_id=%d", msg_id);
        vTaskDelay(1000);
    }
}