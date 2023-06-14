#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "SPI_ws2812b.h"

void app_main(void)
{
    gpio_set_direction(MOSI_PIN, GPIO_MODE_OUTPUT);
    gpio_set_pull_mode(MOSI_PIN, GPIO_PULLDOWN_ONLY);
    initSPI();
    vTaskDelay(1000/portTICK_PERIOD_MS);
    while(1){
        ws2812b_pixel(255,0,0);
        vTaskDelay(1000/portTICK_PERIOD_MS);
        ws2812b_pixel(0,255,0);
        vTaskDelay(1000/portTICK_PERIOD_MS);
        ws2812b_pixel(0,0,255);
        vTaskDelay(1000/portTICK_PERIOD_MS);
    }
    // ws2812b_pixel(255,0,0);
    // vTaskDelay(1000);
    // // Test();1
    // ESP_LOGI(TAG, "Complete Transmits");
}