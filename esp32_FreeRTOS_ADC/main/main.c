#include <stdio.h>
#include <stdlib.h>
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "driver/adc.h"
#include "esp_adc_cal.h"


static const char *TAG_ADC = "ADC";
static const char *TAG_BLINK = "BLINK";
static const char *TAG_COUNT = "COUNT";
static const char *TAG_PARAM = "PARAMS";
static const char *TAG_PARAM_USED = "PARAMS USED";
static esp_adc_cal_characteristics_t adc1_chars;
bool ledState = 0;
int voltage = 0;
int last = 10;
int i = 0;
#define abc ghfg
void vTaskLedBlink(void *pvParameters){
    gpio_set_direction(GPIO_NUM_2, GPIO_MODE_OUTPUT);
    while(1){
        gpio_set_level(GPIO_NUM_2, ledState);
        ledState =! ledState;
        ESP_LOGI(TAG_BLINK, "Led Value: %d", ledState);
        printf("remaining memory of Task Blink : %d byte",uxTaskGetStackHighWaterMark(NULL));
        vTaskDelay(2000/portTICK_PERIOD_MS);
    }
}
void vTaskReadADC(void *pvParameters){
    gpio_set_direction(ADC1_CHANNEL_0, GPIO_MODE_INPUT);
    esp_adc_cal_characterize(ADC_UNIT_1, ADC_ATTEN_11db, ADC_WIDTH_BIT_DEFAULT, 0, &adc1_chars);
    while(1){
        voltage = esp_adc_cal_raw_to_voltage(adc1_get_raw(ADC1_CHANNEL_0), &adc1_chars);
        ESP_LOGI(TAG_ADC, "ADC1 Channel 0: %d mV", voltage);
        printf("remaining memory of Task readADC : %d byte",uxTaskGetStackHighWaterMark(NULL));
        vTaskDelay(200/portTICK_PERIOD_MS);
    }
}
void vTaskUpCounter(void *pvParameters){
    while(1){
        ESP_LOGI(TAG_COUNT, "Count Value: %d", i);
        i++;
        if(i == *((int*) pvParameters)) vTaskDelete(NULL); 
        printf("remaining memory of Task up counter : %d byte",uxTaskGetStackHighWaterMark(NULL));
        vTaskDelay(1000/portTICK_PERIOD_MS);
    }
}
typedef struct{
    int param1;
    float param2;
    char *param3;
} TaskParams;
const char * ROOT_CA =
"-----BEGIN CERTIFICATE-----\n"
"MIIDQTCCAimgAwIBAgITBmyfz5m/jAo54vB4ikPmljZbyjANBgkqhkiG9w0BAQsF\n"
"ADA5MQswCQYDVQQGEwJVUzEPMA0GA1UEChMGQW1hem9uMRkwFwYDVQQDExBBbWF6\n"
"b24gUm9vdCBDQSAxMB4XDTE1MDUyNjAwMDAwMFoXDTM4MDExNzAwMDAwMFowOTEL\n"
"MAkGA1UEBhMCVVMxDzANBgNVBAoTBkFtYXpvbjEZMBcGA1UEAxMQQW1hem9uIFJv\n"
"b3QgQ0EgMTCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBALJ4gHHKeNXj\n"
"ca9HgFB0fW7Y14h29Jlo91ghYPl0hAEvrAIthtOgQ3pOsqTQNroBvo3bSMgHFzZM\n"
"9O6II8c+6zf1tRn4SWiw3te5djgdYZ6k/oI2peVKVuRF4fn9tBb6dNqcmzU5L/qw\n"
"IFAGbHrQgLKm+a/sRxmPUDgH3KKHOVj4utWp+UhnMJbulHheb4mjUcAwhmahRWa6\n"
"VOujw5H5SNz/0egwLX0tdHA114gk957EWW67c4cX8jJGKLhD+rcdqsq08p8kDi1L\n"
"93FcXmn/6pUCyziKrlA4b9v7LWIbxcceVOF34GfID5yHI9Y/QCB/IIDEgEw+OyQm\n"
"jgSubJrIqg0CAwEAAaNCMEAwDwYDVR0TAQH/BAUwAwEB/zAOBgNVHQ8BAf8EBAMC\n"
"AYYwHQYDVR0OBBYEFIQYzIU07LwMlJQuCFmcx7IQTgoIMA0GCSqGSIb3DQEBCwUA\n"
"A4IBAQCY8jdaQZChGsV2USggNiMOruYou6r4lK5IpDB/G/wkjUu0yKGX9rbxenDI\n"
"U5PMCCjjmCXPI6T53iHTfIUJrU6adTrCC2qJeHZERxhlbI1Bjjt/msv0tadQ1wUs\n"
"N+gDS63pYaACbvXy8MWy7Vu33PqUXHeeE6V/Uq2V8viTO96LXFvKWlJbYK8U90vv\n"
"o/ufQJVtMVT8QtPHRh8jrdkPSHCa2XV4cdFyQzR1bldZwgJcJmApzyMZFo6IQ6XU\n"
"5MsI+yMRQ+hDKXJioaldXgjUkK642M4UwtBV8ob2xJNDd2ZhwLnoQdeXeGADbkpy\n"
"rqXRfboQnoZsG4q5WTP468SQvvG5\n"
"-----END CERTIFICATE-----\n";

TaskParams params = {1, 2.5, "-----BEGIN CERTIFICATE-----\n"
"MIIDQTCCAimgAwIBAgITBmyfz5m/jAo54vB4ikPmljZbyjANBgkqhkiG9w0BAQsF\n"
"ADA5MQswCQYDVQQGEwJVUzEPMA0GA1UEChMGQW1hem9uMRkwFwYDVQQDExBBbWF6\n"
"b24gUm9vdCBDQSAxMB4XDTE1MDUyNjAwMDAwMFoXDTM4MDExNzAwMDAwMFowOTEL\n"
"MAkGA1UEBhMCVVMxDzANBgNVBAoTBkFtYXpvbjEZMBcGA1UEAxMQQW1hem9uIFJv\n"
"b3QgQ0EgMTCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBALJ4gHHKeNXj\n"
"ca9HgFB0fW7Y14h29Jlo91ghYPl0hAEvrAIthtOgQ3pOsqTQNroBvo3bSMgHFzZM\n"
"9O6II8c+6zf1tRn4SWiw3te5djgdYZ6k/oI2peVKVuRF4fn9tBb6dNqcmzU5L/qw\n"
"IFAGbHrQgLKm+a/sRxmPUDgH3KKHOVj4utWp+UhnMJbulHheb4mjUcAwhmahRWa6\n"
"VOujw5H5SNz/0egwLX0tdHA114gk957EWW67c4cX8jJGKLhD+rcdqsq08p8kDi1L\n"
"93FcXmn/6pUCyziKrlA4b9v7LWIbxcceVOF34GfID5yHI9Y/QCB/IIDEgEw+OyQm\n"
"jgSubJrIqg0CAwEAAaNCMEAwDwYDVR0TAQH/BAUwAwEB/zAOBgNVHQ8BAf8EBAMC\n"
"AYYwHQYDVR0OBBYEFIQYzIU07LwMlJQuCFmcx7IQTgoIMA0GCSqGSIb3DQEBCwUA\n"
"A4IBAQCY8jdaQZChGsV2USggNiMOruYou6r4lK5IpDB/G/wkjUu0yKGX9rbxenDI\n"
"U5PMCCjjmCXPI6T53iHTfIUJrU6adTrCC2qJeHZERxhlbI1Bjjt/msv0tadQ1wUs\n"
"N+gDS63pYaACbvXy8MWy7Vu33PqUXHeeE6V/Uq2V8viTO96LXFvKWlJbYK8U90vv\n"
"o/ufQJVtMVT8QtPHRh8jrdkPSHCa2XV4cdFyQzR1bldZwgJcJmApzyMZFo6IQ6XU\n"
"5MsI+yMRQ+hDKXJioaldXgjUkK642M4UwtBV8ob2xJNDd2ZhwLnoQdeXeGADbkpy\n"
"rqXRfboQnoZsG4q5WTP468SQvvG5\n"
"-----END CERTIFICATE-----\n"};

void vTaskDisplayParams(void *pvParameters){
    TaskParams *params = (TaskParams *) pvParameters;
    int p1 = params ->param1;
    float p2 = params ->param2;
    char *p3 = params ->param3;
    while(1){
        ESP_LOGI(TAG_PARAM, "Param 1: %d || Param 2: %f || Param 3: %s", p1, p2, p3);
        printf("remaining memory of Task Param : %d byte",uxTaskGetStackHighWaterMark(NULL));
        vTaskDelay(1000/portTICK_PERIOD_MS);
    }
}

void app_main(void)
{
    xTaskCreate(&vTaskLedBlink, "vTaskLedBlink", 2048, NULL, 5, NULL);
    xTaskCreate(&vTaskReadADC, "vTaskReadADC", 2048, NULL, 5, NULL);
    xTaskCreate(&vTaskUpCounter, "vTaskUpCounter", 2048, (void*)&last, 5, NULL);
    xTaskCreate(&vTaskDisplayParams, "vTaskDisplayParams", 2048, &params, 5, NULL);
}
