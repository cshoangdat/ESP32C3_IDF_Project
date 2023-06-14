#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/ledc.h"

#define LEDC_TIMER LEDC_TIMER_0
#define LEDC_MODE LEDC_LOW_SPEED_MODE
#define LEDC_OUPUT GPIO_NUM_5
#define LEDC_CHANNEL LEDC_CHANNEL_0
#define LEDC_DUTY_RES LEDC_TIMER_8_BIT
#define LEDC_FREQUENCY 5000

void ledc_init(void){
    ledc_timer_config_t ledc_timer = {
        .speed_mode = LEDC_MODE,
        .duty_resolution = LEDC_DUTY_RES,
        .timer_num = LEDC_TIMER,
        .freq_hz = LEDC_FREQUENCY
    };
    ledc_timer_config(&ledc_timer);
    ledc_channel_config_t ledc_channel = {
        .channel = LEDC_CHANNEL,
        .gpio_num = LEDC_OUPUT,
        .speed_mode = LEDC_MODE,
        .duty = 0,
        .timer_sel = LEDC_TIMER
    };
    ledc_channel_config(&ledc_channel);
    printf("Complete Init");
}

void adc_write(int value){
    ledc_set_duty(LEDC_MODE, LEDC_CHANNEL, value);
    ledc_update_duty(LEDC_MODE, LEDC_CHANNEL);
}

void RGB_led(int red, int green, int blue){
    adc_write(red);
    adc_write(blue);
    adc_write(green);
}

void app_main(void)
{
    ledc_init();
    
    while(1){
        // RGB_led(255,0,0);
        adc_write(128);
        printf("%d", ledc_get_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL));
        vTaskDelay(1000/portTICK_PERIOD_MS);
        adc_write(0);
        printf("%d", ledc_get_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL));
        vTaskDelay(1000/portTICK_PERIOD_MS);
    }
}
