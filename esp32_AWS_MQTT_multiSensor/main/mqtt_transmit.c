#include "mqtt_transmit.h"
static const char *TAG = "AWS_MQTT";
const char* TOPIC = "Data";
const char* TOPIC_RECIEVE = "Recieve";

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
        esp_mqtt_client_subscribe(client, TOPIC_RECIEVE, 0);
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
        // if (strncmp(event->data, "LED ON", event->data_len) == 0) {
        //     ESP_LOGI(TAG, "TURN LED ON");
        //     gpio_set_direction(GPIO_NUM_2, GPIO_MODE_OUTPUT);
        //     gpio_set_level(GPIO_NUM_2, 1);
        // }
        // if(strncmp(event->data, "LED OFF", event->data_len) == 0){
        //     ESP_LOGI(TAG, "TURN LED OFF");
        //     gpio_set_direction(GPIO_NUM_2, GPIO_MODE_OUTPUT);
        //     gpio_set_level(GPIO_NUM_2, 0);
        // }
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

void mqtt_app_start(const char* URI)
{
    const esp_mqtt_client_config_t mqtt_cfg = {
        .uri = URI,
        .port = 8883,
        .cert_pem = (const char*) cert_start,
        .client_cert_pem = (const char*) certificate_start,
        .client_key_pem = (const char*) private_start,
        .event_handle = mqtt_event_handler,
        .event_loop_handle = mqtt_event_handler
    };
    client = esp_mqtt_client_init(&mqtt_cfg);
    esp_mqtt_client_register_event(client, ESP_EVENT_ANY_ID, mqtt_event_handler, NULL);
    esp_mqtt_client_start(client);
}