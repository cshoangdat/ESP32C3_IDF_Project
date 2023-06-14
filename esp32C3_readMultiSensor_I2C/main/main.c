#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <string.h>
#include <esp_log.h>
#include <esp_err.h>
#include <esp_system.h>
#include "ltr303.h"
#include <scd4x.h>
#include <sht4x.h>
#define SDA_PIN GPIO_NUM_4
#define SCL_PIN GPIO_NUM_5

static const char *TAG_SCD4X = "SCD4X";
static const char *TAG_LTR303 = "LTR303";
static const char *TAG_SHT4X = "SHT4X";

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
        vTaskDelayUntil(&last_wakeup, pdMS_TO_TICKS(2000));
    }
}

void taskReadSCD4x(void *pvParameter){
    ESP_ERROR_CHECK(scd4x_init_desc(&scd4x_dev, 0, SDA_PIN, SCL_PIN));
    uint16_t co2;
    float temperature, humidity;
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
        ESP_LOGI(TAG_SCD4X, "CO2: %u ppm", co2);
    }
}

void taskReadLTR303(void *pvParameter){
    uint16_t data_ch0, data_ch1;
    uint32_t data;
    ESP_ERROR_CHECK(ltr303_set_gain(LTR303_GAIN_2));
    ESP_LOGI(TAG_LTR303, "ltr303 set gain complete");
    ESP_ERROR_CHECK(ltr303_set_intergrationTime(LTR303_INTEGTIME_200));
    ESP_LOGI(TAG_LTR303, "ltr303 set intergration time complete");
    while(1){
        ESP_ERROR_CHECK(ltr303_register_read(LTR303_ALS_DATA_CH1_RESULT, &data, 4));
        data_ch0 = data >>16;
        data_ch1 = data & 0xFFFF;
        ESP_LOGI(TAG_LTR303, "CH0: %d\t CH1: %d \t LUX: %.0f", data_ch0, data_ch1, ltr303_value(data_ch0, data_ch1, LTR303_GAIN_2, LTR303_INTEGTIME_200));
        vTaskDelay(500);
    }
}

void app_main(void)
{
    ESP_ERROR_CHECK(i2cdev_init());
    xTaskCreate(taskReadSHT4x, "read SHT4x", configMINIMAL_STACK_SIZE * 8, NULL, 5, NULL);
    xTaskCreate(taskReadSCD4x, "read SCD4x", configMINIMAL_STACK_SIZE * 8, NULL, 5, NULL);
    xTaskCreate(taskReadLTR303, "read LTR303", configMINIMAL_STACK_SIZE * 8, NULL, 5, NULL);
}
