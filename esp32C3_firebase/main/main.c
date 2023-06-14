#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "wifi_connect.h"
#include "http_transmit.h"
#include <esp_log.h>
#include <esp_err.h>
#include <esp_system.h>
#include "ltr303.h"
#include <scd4x.h>
#include <sht4x.h>

const char* ESP_WIFI_SSID =                             "Ambi";
const char* ESP_WIFI_PASS =                             "1234ambi";
const char* FIREBASE_HOST =                             "https://esp-idf-project-default-rtdb.firebaseio.com/";
const char* FIREBASE_AUTH =                             "3PVyrB21rHGcv0rfAHWPQmQUsoKWLLyAYtMSL9M3";
const char* URL =                                       "https://esp-idf-project-default-rtdb.firebaseio.com/.json?auth=3PVyrB21rHGcv0rfAHWPQmQUsoKWLLyAYtMSL9M3";
char data_SHT4x[50];
char data_SCD4x[50];
char data_LTR303[50];


#define SDA_PIN                                         GPIO_NUM_4
#define SCL_PIN                                         GPIO_NUM_5

static const char *TAG_SCD4X =                          "SCD4X";
static const char *TAG_LTR303 =                         "LTR303";
static const char *TAG_SHT4X =                          "SHT4X";

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
        ESP_LOGI(TAG_SHT4X,"sht4x Sensor: %.2f °C, %.2f %%", temperature, humidity);
        memset(data_SHT4x, 0, sizeof(data_SHT4x));
        sprintf(data_SHT4x,"{\"Temperature\": %.2f,\"Humidity\": %.2f}", temperature, humidity);
        client_patch_rest_function(URL, data_SHT4x);
        // printf("remaining memory of task Read SHT4x : %d byte",uxTaskGetStackHighWaterMark(NULL));
        vTaskDelay(2000/portTICK_PERIOD_MS);
    }
}

void taskReadSCD4x(void *pvParameter){
    ESP_ERROR_CHECK(scd4x_init_desc(&scd4x_dev, 0, SDA_PIN, SCL_PIN));
    uint16_t co2;
    float temperature, humidity;
    ESP_LOGI(TAG, "Initializing sensor...");
    ESP_ERROR_CHECK(scd4x_wake_up(&scd4x_dev));
    ESP_ERROR_CHECK(scd4x_stop_periodic_measurement(&scd4x_dev));
    ESP_ERROR_CHECK(scd4x_reinit(&scd4x_dev));
    ESP_LOGI(TAG, "Sensor initialized");

    uint16_t serial[3];
    ESP_ERROR_CHECK(scd4x_get_serial_number(&scd4x_dev, serial, serial + 1, serial + 2));
    ESP_LOGI(TAG, "Sensor serial number: 0x%04x%04x%04x", serial[0], serial[1], serial[2]);

    ESP_ERROR_CHECK(scd4x_start_periodic_measurement(&scd4x_dev));
    ESP_LOGI(TAG, "Periodic measurements started");
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
        sprintf(data_SCD4x,"{\"Co2\": %u}", co2);
        client_patch_rest_function(URL, data_SCD4x);
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
        sprintf(data_LTR303,"{\"CH0\": %u,\"CH1\": %u,\"Lux\": %.0f}", data_ch0, data_ch1, ltr303_value(data_ch0, data_ch1, LTR303_GAIN_2, LTR303_INTEGTIME_200));
        client_patch_rest_function(URL, data_LTR303);
        ESP_LOGI(TAG_LTR303, "CH0: %u\t CH1: %u \t Lux: %.0f", data_ch0, data_ch1, ltr303_value(data_ch0, data_ch1, LTR303_GAIN_2, LTR303_INTEGTIME_200));
        // printf("remaining memory of task Read LTR303 : %d byte",uxTaskGetStackHighWaterMark(NULL));
        vTaskDelay(4000/portTICK_PERIOD_MS);
    }
}

void app_main(void)
{
    wifi_connect(ESP_WIFI_SSID, ESP_WIFI_PASS);
    vTaskDelay(2000/portTICK_PERIOD_MS);
    ESP_ERROR_CHECK(i2cdev_init());
    xTaskCreate(taskReadSHT4x, "read SHT4x", 3072, NULL, 5, NULL);
    xTaskCreate(taskReadSCD4x, "read SCD4x", 3072, NULL, 5, NULL);
    xTaskCreate(taskReadLTR303, "read LTR303", 3072, NULL, 5, NULL);

}

