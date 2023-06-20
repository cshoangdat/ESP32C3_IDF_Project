#ifndef MAIN_SPI_WS2812B_H
#define MAIN_SPI_WS2812B_H
#include <stdio.h>
#include <string.h>
#include "driver/spi_master.h"
#include "driver/gpio.h"
#include "sdkconfig.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
// #define MOSI_PIN GPIO_NUM_8
#define MOSI_PIN GPIO_NUM_3
#define TAG_LED "SPI"
#define bit_zero 0xC0//0xC0 
#define bit_one 0xFC //0xF8 0xFC

void initSPI(void);
void sendData_8bit(uint8_t data);
void sendData_24byte(unsigned char data);
void sendData_2byte(unsigned long data);
void ws2812b_pixel(unsigned char red, unsigned char green, unsigned char blue);
void test(unsigned char red, unsigned char green, unsigned char blue);
void Test(void);
void sendDataReset(void);
bool is_init_SPI_done(void);
#endif