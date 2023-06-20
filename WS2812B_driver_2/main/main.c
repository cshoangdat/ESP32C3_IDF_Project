#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "ws2812B_driver.h"

void app_main(void)
{
    init_spi();
    while(1){
        ESP_LOGI("DONE", "BLUE");
        CRGB a = {.r = 0, .g = 0, .b = 255};
        update_led_strip((uint32_t)&a);
        vTaskDelay(1000);
        ESP_LOGI("DONE", "GREEN");
        CRGB b = {.r = 0, .g = 255, .b = 0};
        update_led_strip((uint32_t)&b);
        vTaskDelay(1000);
        ESP_LOGI("DONE", "RED");
        CRGB c = {.r = 255, .g = 0, .b = 0};
        update_led_strip((uint32_t)&c);
        vTaskDelay(1000);
        ESP_LOGI("DONE", "WHITE");
        CRGB d = {.r = 255, .g = 255, .b = 255};
        update_led_strip((uint32_t)&d);
        vTaskDelay(1000);
        ESP_LOGI("DONE", "BLACK");
        CRGB e = {.r = 0, .g = 0, .b = 0};
        update_led_strip((uint32_t)&e);
        vTaskDelay(1000);
    }
    ESP_LOGI("DONE", "DONE");
}
