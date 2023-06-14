#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "sdkconfig.h"
#include "led_strip.h"

static const char *TAG = "LED RGB";

/* Use project configuration menu (idf.py menuconfig) to choose the GPIO to blink,
   or you can edit the following line and set a number here.
*/
#define BLINK_GPIO GPIO_NUM_8

static led_strip_t *pStrip_a;

void ledRGB(void){
    // pStrip_a->set_pixel(pStrip_a, 0, 16, 16, 16);
    // pStrip_a->refresh(pStrip_a, 100);
    // vTaskDelay(1000/portTICK_PERIOD_MS);
    pStrip_a->set_pixel(pStrip_a, 0, 96, 0, 0);
    pStrip_a->refresh(pStrip_a, 100);
    vTaskDelay(1000/portTICK_PERIOD_MS);
    pStrip_a->set_pixel(pStrip_a, 0, 0, 255, 0);
    pStrip_a->refresh(pStrip_a, 100);
    vTaskDelay(1000/portTICK_PERIOD_MS);
    pStrip_a->set_pixel(pStrip_a, 0, 0, 0, 255);
    pStrip_a->refresh(pStrip_a, 100);
    vTaskDelay(1000/portTICK_PERIOD_MS);
    pStrip_a->set_pixel(pStrip_a, 0, 0, 0, 0);
    pStrip_a->refresh(pStrip_a, 100);
    vTaskDelay(1000/portTICK_PERIOD_MS);
    pStrip_a->set_pixel(pStrip_a, 0, 255, 255, 255);
    pStrip_a->refresh(pStrip_a, 100);
    vTaskDelay(1000/portTICK_PERIOD_MS);
    pStrip_a->set_pixel(pStrip_a, 0, 255, 255, 0);
    pStrip_a->refresh(pStrip_a, 100);
    vTaskDelay(1000/portTICK_PERIOD_MS);
}
void configure_led(void)
{
    ESP_LOGI(TAG, "Example configured to blink addressable LED!");
    /* LED strip initialization with the GPIO and pixels number*/
    pStrip_a = led_strip_init(0, BLINK_GPIO, 1);
    /* Set all LED off to clear all pixels */
    pStrip_a->clear(pStrip_a, 50);
}

void app_main(void)
{
    configure_led();
    while(1){
        ledRGB();
    }
}
