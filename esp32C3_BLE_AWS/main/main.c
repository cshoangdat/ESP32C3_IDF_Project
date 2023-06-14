#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "wifi_connect.h"
#include "ble_connect.h"
#include "mqtt_transmit.h"

#include "ltr303.h"
#include <scd4x.h>
#include <sht4x.h>

const char* URI =                                       "mqtts://a3suuuxay09k3c-ats.iot.us-east-2.amazonaws.com";
char data_SHT4x[50];
char data_SCD4x[50];
char data_LTR303[50];
char data_Sensor[150];
#define SDA_PIN                                         GPIO_NUM_4
#define SCL_PIN                                         GPIO_NUM_5
static const char *TAG =                                "AWS_MQTT";
static const char *TAG_SCD4X =                          "SCD4X";
static const char *TAG_LTR303 =                         "LTR303";
static const char *TAG_SHT4X =                          "SHT4X";
static const char *TAG_SENSOR =                         "SENSOR";
static sht4x_t sht4x_dev;
static i2c_dev_t scd4x_dev = { 0 };

void taskReadSHT4x(void *pvParameter){
    memset(&sht4x_dev, 0, sizeof(sht4x_t));
    ESP_ERROR_CHECK(sht4x_init_desc(&sht4x_dev, 0, SDA_PIN, SCL_PIN));
    ESP_ERROR_CHECK(sht4x_init(&sht4x_dev));

    float temperature;
    float humidity;

    TickType_t last_wakeup = xTaskGetTickCount();
    while (1)
    {
        ESP_ERROR_CHECK(sht4x_measure(&sht4x_dev, &temperature, &humidity));
        memset(data_SHT4x, 0, sizeof(data_SHT4x));
        sprintf(data_SHT4x,"{\"Temperature\": \"%.2f\",\"Humidity\": \"%.2f\",", temperature, humidity);
        // int msg_id = esp_mqtt_client_publish(client, "Data", data_SHT4x, 0, 0, 0);
        // ESP_LOGI(TAG_SHT4X, "sent publish SHT4X successful, msg_id=%d", msg_id);
        ESP_LOGI(TAG_SHT4X,"sht4x Sensor: %.2f °C, %.2f %%", temperature, humidity);
        // printf("remaining memory of task Read SHT4x : %d byte",uxTaskGetStackHighWaterMark(NULL));
        vTaskDelay(2000/portTICK_PERIOD_MS);
    }
}

void taskReadSCD4x(void *pvParameter){
    ESP_ERROR_CHECK(scd4x_init_desc(&scd4x_dev, 0, SDA_PIN, SCL_PIN));
    uint16_t co2;
    float temperature, humidity;
    // ESP_LOGI(TAG_SCD4X, "Initializing sensor...");
    // ESP_ERROR_CHECK(scd4x_wake_up(&scd4x_dev));
    // ESP_ERROR_CHECK(scd4x_stop_periodic_measurement(&scd4x_dev));
    // ESP_ERROR_CHECK(scd4x_reinit(&scd4x_dev));
    // ESP_LOGI(TAG_SCD4X, "Sensor initialized");

    // uint16_t serial[3];
    // ESP_ERROR_CHECK(scd4x_get_serial_number(&scd4x_dev, serial, serial + 1, serial + 2));
    // ESP_LOGI(TAG_SCD4X, "Sensor serial number: 0x%04x%04x%04x", serial[0], serial[1], serial[2]);

    // ESP_ERROR_CHECK(scd4x_start_periodic_measurement(&scd4x_dev));
    // ESP_LOGI(TAG_SCD4X, "Periodic measurements started");
    while (1)
    {
        vTaskDelay(5000/portTICK_PERIOD_MS);
        esp_err_t res = scd4x_read_measurement(&scd4x_dev, &co2, &temperature, &humidity);
        if (res != ESP_OK)
        {
            ESP_LOGE(TAG_SCD4X, "Error reading results %d (%s)", res, esp_err_to_name(res));
            continue;
        }

        if (co2 == 0)
        {
            ESP_LOGW(TAG_SCD4X, "Invalid sample detected, skipping");
            continue;
        }
        memset(data_SCD4x, 0, sizeof(data_SCD4x));
        sprintf(data_SCD4x,"\"Co2\": \"%u\",", co2);
        // int msg_id = esp_mqtt_client_publish(client, "Data", data_SCD4x, 0, 0, 0);
        // ESP_LOGI(TAG_SCD4X, "sent publish SCD4X successful, msg_id=%d", msg_id);
        ESP_LOGI(TAG_SCD4X, "Co2: %u ppm", co2);
        // printf("remaining memory of task Read SCD4x : %d byte",uxTaskGetStackHighWaterMark(NULL));
    }
}

void taskReadLTR303(void *pvParameter){
    uint16_t data_ch0, data_ch1;
    uint32_t data_lux;
    ESP_ERROR_CHECK(ltr303_set_gain(LTR303_GAIN_2));
    ESP_LOGI(TAG_LTR303, "ltr303 set gain complete");
    ESP_ERROR_CHECK(ltr303_set_intergrationTime(LTR303_INTEGTIME_200));
    ESP_LOGI(TAG_LTR303, "ltr303 set intergration time complete");
    while(1){
        ESP_ERROR_CHECK(ltr303_register_read(LTR303_ALS_DATA_CH1_RESULT, &data_lux, 4));
        data_ch0 = data_lux >>16;
        data_ch1 = data_lux & 0xFFFF;
        vTaskDelay(500/portTICK_PERIOD_MS);
        memset(data_LTR303, 0, sizeof(data_LTR303));
        sprintf(data_LTR303,"\"CH0\": \"%u\",\"CH1\": \"%u\",\"Lux\": \"%.0f\"}", data_ch0, data_ch1, ltr303_value(data_ch0, data_ch1, LTR303_GAIN_2, LTR303_INTEGTIME_200));
        // int msg_id = esp_mqtt_client_publish(client, "Data", data_LTR303, 0, 0, 0);
        // ESP_LOGI(TAG_LTR303, "sent publish LTR303 successful, msg_id=%d", msg_id);
        ESP_LOGI(TAG_LTR303, "CH0: %u\t CH1: %u \t Lux: %.0f", data_ch0, data_ch1, ltr303_value(data_ch0, data_ch1, LTR303_GAIN_2, LTR303_INTEGTIME_200));
        vTaskDelay(4000/portTICK_PERIOD_MS);
    }
}

void taskTransmitToAWS(void *pvParamenter){
    while(1){
        if(data_LTR303 != NULL && data_SCD4x != NULL && data_SHT4x != NULL){
           memset(data_Sensor, 0 , sizeof(data_Sensor));
           sprintf(data_Sensor,"%s%s%s", data_SHT4x, data_SCD4x, data_LTR303);
           int msg_id = esp_mqtt_client_publish(client, "Data", data_Sensor, 0, 0, 0);
           ESP_LOGI(TAG_SENSOR,"sent publish Data successful, msg_id=%d", msg_id);
           ESP_LOGI(TAG_SENSOR,"Data Sensor:%s", data_Sensor);
           vTaskDelay(3000/portTICK_PERIOD_MS);
        }
    }
}

void app_main(void)
{
    ble_connect();
    ESP_LOGI("BLE", "BLE_DONE");
    ESP_ERROR_CHECK(i2cdev_init());
    while(esp_wifi_connect() != ESP_OK){
        ESP_LOGI("BLE_WIFI", "Waiting for Wifi...");
        vTaskDelay(500/portTICK_PERIOD_MS);
        if(esp_wifi_connect() == ESP_OK){
            mqtt_app_start(URI);
            break;
        }
    }
    xTaskCreate(taskReadSHT4x, "read SHT4x", 3072, NULL, 5, NULL);
    xTaskCreate(taskReadSCD4x, "read SCD4x", 3072, NULL, 5, NULL);
    xTaskCreate(taskReadLTR303, "read LTR303", 3072, NULL, 5, NULL);
    xTaskCreate(taskTransmitToAWS, "transmit to AWS", 3072, NULL, 5, NULL);
}
