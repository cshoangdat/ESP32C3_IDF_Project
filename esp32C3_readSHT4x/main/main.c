#include <stdio.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <esp_system.h>
#include <sht4x.h>
#include <string.h>
#include <esp_err.h>
#define SDA_PIN GPIO_NUM_4
#define SCL_PIN GPIO_NUM_5
#ifndef APP_CPU_NUM
#define APP_CPU_NUM PRO_CPU_NUM
#endif

static sht4x_t dev;

void task(void *pvParameters)
{
    float temperature;
    float humidity;

    TickType_t last_wakeup = xTaskGetTickCount();

    while (1)
    {
        // perform one measurement and do something with the results
        ESP_ERROR_CHECK(sht4x_measure(&dev, &temperature, &humidity));
        printf("sht4x Sensor: %.2f °C, %.2f %%\n", temperature, humidity);

        // wait until 5 seconds are over
        vTaskDelayUntil(&last_wakeup, pdMS_TO_TICKS(5000));
    }
}

void app_main()
{
    ESP_ERROR_CHECK(i2cdev_init());
    memset(&dev, 0, sizeof(sht4x_t));

    ESP_ERROR_CHECK(sht4x_init_desc(&dev, 0, SDA_PIN, SCL_PIN));
    ESP_ERROR_CHECK(sht4x_init(&dev));

    xTaskCreatePinnedToCore(task, "sht4x_test", configMINIMAL_STACK_SIZE * 8, NULL, 5, NULL, APP_CPU_NUM);
}
