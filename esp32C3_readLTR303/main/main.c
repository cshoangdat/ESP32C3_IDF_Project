#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "ltr303.h"
#define TAG "LTR303"

void app_main(void)
{
    uint16_t data_ch0, data_ch1;
    uint32_t data;
    ESP_ERROR_CHECK(i2c_master_init());
    ESP_LOGI(TAG, "I2C init complete");
    ESP_ERROR_CHECK(ltr303_set_gain(LTR303_GAIN_2));
    ESP_LOGI(TAG, "ltr303 set gain complete");
    ESP_ERROR_CHECK(ltr303_set_intergrationTime(LTR303_INTEGTIME_200));
    ESP_LOGI(TAG, "ltr303 set intergration time complete");
    while(1){
        ESP_ERROR_CHECK(ltr303_register_read(LTR303_ALS_DATA_CH1_RESULT, &data, 4));
        data_ch0 = data >>16;
        data_ch1 = data & 0xFFFF;
        ESP_LOGI(TAG, "CH0: %d\n", data_ch0);
        ESP_LOGI(TAG, "CH1: %d\n", data_ch1);
        ESP_LOGI(TAG, "LUX: %d\n", ltr303_value(data_ch0, data_ch1, LTR303_GAIN_2, LTR303_INTEGTIME_200));
        vTaskDelay(500);
    }
}
