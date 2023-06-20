#ifndef WS2812B_DRIVER_H
#define WS2812B_DRIVER_H
#include <stdio.h>
#include <string.h>
#include "driver/spi_master.h"
#include "driver/gpio.h"
#include "sdkconfig.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#define MOSI_PIN GPIO_NUM_10
#define CLOCK_SPEED_HZ 3200000
#define SPI_TAG "SPI"

#define LED_MAX_NBER_LEDS 24*2
#define LED_DMA_BUFFER_SIZE ((LED_MAX_NBER_LEDS * 16 * (24/4)))+1

typedef struct{
    spi_host_device_t host;
    spi_device_handle_t spi;
    spi_device_interface_config_t devcfg;
    spi_bus_config_t buscfg;
} SPI_setting_t;

static SPI_setting_t SPI_setting = {
    .host = SPI2_HOST,
    .buscfg = {
        .mosi_io_num = MOSI_PIN,
        .max_transfer_sz = LED_DMA_BUFFER_SIZE,
        .miso_io_num = -1,
        .sclk_io_num = -1,
        .quadhd_io_num = -1,
        .quadwp_io_num = -1,
    },
    .devcfg = {
        .mode = 0,
        .clock_speed_hz = CLOCK_SPEED_HZ,
        .spics_io_num = -1,
        .queue_size = 1,
        .command_bits = 0,
        .address_bits = 0
    }
};

typedef struct CRGB {
        uint8_t r;
        uint8_t g;
        uint8_t b;
}CRGB;

uint16_t* led_DMA_buffer;

void init_spi(void);
void update_led_strip(uint32_t* bufLed);
// void fill_buffer(uint32_t* bufLed);

#endif