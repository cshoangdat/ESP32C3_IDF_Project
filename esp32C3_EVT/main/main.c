#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "driver/uart.h"
#include "driver/gpio.h"
#include "sdkconfig.h"
#include "esp_log.h"
#include <esp_err.h>
#include "esp_system.h"

#include "ltr303.h"
#include "mqtt_transmit.h"
#include "wifi_connect.h"
#include "ble_connect.h"
#include "SPI_ws2812b.h"

#define UART_TXD                                        4
#define UART_RXD                                        5
#define UART_RTS                                        18
#define UART_CTS                                        19

#define UART_PORT_NUM                                   UART_NUM_1
#define UART_BAUD_RATE                                  115200
#define TASK_STACK_SIZE                                 2048

static const char *TAG =                                "TEST CASE";
const char* URI =                                       "mqtts://a3suuuxay09k3c-ats.iot.us-east-2.amazonaws.com";
char data_aws[50];

#define BUF_SIZE (1024)
uint8_t *data_uart;
char* SSID = NULL;
char* PASS = NULL;
bool wifi_connected = 0;

enum{
    ENTER = 1,
    TC_WIFI_CONNECT,
    TC_AWS_TRANSMIT,
    TC_LED,
    TC_BLE,
};

char* label[] = {
    "\n",
    "wifi\n",
    "aws\n",
    "led\n",
    "ble\n",
};

char* readData(uart_port_t port){
    int len = uart_read_bytes(UART_PORT_NUM, data_uart, (BUF_SIZE - 1), 20 / portTICK_RATE_MS);
    uart_write_bytes(UART_PORT_NUM, (const char *) data_uart, len);
    char *a = (char *) malloc(len + 1);
    strncpy(a, (const char *) data_uart, len);
    a[len] = '\0';
    return a;
}

uint8_t chooseMode(void){
    char* mode = readData(UART_PORT_NUM);
    uint8_t result = 0;
    for(int i = 0; i < sizeof(label)/sizeof(label[0]); i ++ ){
        if(strcmp(mode, label[i]) == 0){
            result = i+1;
            break;
        }
    }
    return result;
}

void app_main(void){
    const uart_config_t uart_config = {
    .baud_rate = UART_BAUD_RATE,
    .data_bits = UART_DATA_8_BITS,
    .parity    = UART_PARITY_DISABLE,
    .stop_bits = UART_STOP_BITS_1,
    .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
    .source_clk = UART_SCLK_APB,
    };
    int intr_alloc_flags = 0;
    ESP_ERROR_CHECK(uart_driver_install(UART_PORT_NUM, BUF_SIZE * 2, 0, 0, NULL, intr_alloc_flags));
    ESP_ERROR_CHECK(uart_param_config(UART_PORT_NUM, &uart_config));
    ESP_ERROR_CHECK(uart_set_pin(UART_PORT_NUM, UART_TXD, UART_RXD, UART_RTS, UART_CTS));
    data_uart = (uint8_t *) malloc(BUF_SIZE);
    while (1) {
        uint8_t mode = chooseMode();
        switch(mode){
            case ENTER:
                ESP_LOGI(TAG, "Choose Mode");
                while(chooseMode() == ENTER);
            break;
            case TC_WIFI_CONNECT:
                ESP_LOGI("WIFI", "Test Wifi connect");
                SSID = NULL;
                PASS = NULL;
                ESP_LOGI("WIFI", "YOUR SSID WIFI: ");
                while(SSID == NULL || strlen(SSID) == 0){
                    SSID = readData(UART_PORT_NUM);
                    if(strstr(SSID, "\n") != NULL) strtok(SSID,"\n");
                }
                ESP_LOGI("WIFI", "SSID: %s",SSID);

                ESP_LOGI("WIFI", "YOUR PASS WIFI: ");
                while(PASS == NULL || strlen(PASS) == 0){
                    PASS = readData(UART_PORT_NUM);
                    if(strstr(PASS, "\n") != NULL) strtok(PASS,"\n");
                }
                ESP_LOGI("WIFI", "PASS: %s",PASS);

                if((SSID != NULL && strlen(SSID) > 0) && (PASS != NULL && strlen(PASS) > 0)){
                    if(is_wifi_connected() == false){
                        set_up_wifi();
                        wifi_connect(SSID, PASS);
                    }
                    else{
                        wifi_disconnect();
                        wifi_connect(SSID, PASS);
                    }
                }
                while(chooseMode() != ENTER);
            break;
            case TC_AWS_TRANSMIT:
                ESP_LOGI(TAG, "Test AWS Transmit");
                if(is_wifi_connected() == true){
                    mqtt_app_start(URI);
                    for(int i = 0; i < 3 ; i++){
                        sprintf(data_aws,"{\"Temp Data\": \"%d\"}",i );
                        int msg_id = esp_mqtt_client_publish(client, "Data", data_aws, 0, 0, 0);
                        ESP_LOGI(TAG, "sent publish successful, msg_id=%d", msg_id);
                        if(chooseMode() == ENTER) break;
                        vTaskDelay(500);
                    }
                    mqtt_app_stop();
                    ESP_LOGI(TAG, "Test AWS Transmit Done");
                }
                else{
                    ESP_LOGI(TAG, "Back to set WiFi First");
                }
                while(chooseMode() != ENTER);
            break;
            case TC_BLE:
                ESP_LOGI(TAG, "Test BLE");
                if(is_ble_init_done() == false){
                    ble_init();
                    ESP_LOGI(TAG, "BLE init done");
                }
                if(is_ble_registered() == false){
                    ble_register();
                    ESP_LOGI(TAG, "BLE start");
                }
                else{
                    ble_unregister();
                    ESP_LOGI(TAG, "BLE unregister");
                    ble_register();
                    ESP_LOGI(TAG, "BLE start");
                }
                while(chooseMode() != ENTER){
                    if(chooseMode() == ENTER) ble_unregister();
                }
            break;
            case TC_LED:
                ESP_LOGI(TAG, "Test LED");
                if(is_init_SPI_done() == false){
                    initSPI();
                    vTaskDelay(500/portTICK_PERIOD_MS);
                } 
                while(chooseMode() != ENTER){
                    ws2812b_pixel(255,0,0);
                    vTaskDelay(100/portTICK_PERIOD_MS);
                    ws2812b_pixel(0,255,0);
                    vTaskDelay(100/portTICK_PERIOD_MS);
                    ws2812b_pixel(0,0,255);
                    vTaskDelay(100/portTICK_PERIOD_MS);
                }
                if(chooseMode() == ENTER) ws2812b_pixel(0,0,0);
            break;
            default:
            break;
        }
    }
}