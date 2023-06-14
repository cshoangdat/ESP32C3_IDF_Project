#ifndef LTR303_DRIVER_H_
#define LTR303_DRIVER_H_

#include <esp_system.h>
#include <sdkconfig.h>
#include <stdio.h>
#include <driver/i2c.h>
#include <esp_log.h>

#define I2C_MASTER_SDA_PIN 4
#define I2C_MASTER_SCL_PIN 5
#define I2C_MASTER_NUM 0
#define I2C_MASTER_FREQ_HZ 400000
#define I2C_MASTER_TX_BUF_DISABLE 0
#define I2C_MASTER_RX_BUF_DISABLE 0
#define I2C_MASTER_TIMEOUT 1000
#define I2C_WRITE_BIT I2C_MASTER_WRITE
#define I2C_READ_BIT I2C_MASTER_READ
#define ACK_VAL 0x0
#define NACK_VAL 0x1
#define ACK_CHECK_EN 0x1
#define ACK_CHECK_DIS 0x0

#define LTR303_ALS_DATA_CH1_RESULT 0X88
#define LTR303_PART_ID 0x86
#define LTR303_I2C_ADRESS 0x29
#define LTR303_MANU_ID 0x87
#define LTR303_ALS_CTRL 0x80
#define LTR303_STATUS 0x8C
#define LTR303_MEAS_RATE 0x85

typedef enum{
    LTR303_GAIN_1 = 0,
    LTR303_GAIN_2 = 1,
    LTR303_GAIN_4 = 2,
    LTR303_GAIN_8 = 3,
    LTR303_GAIN_48 = 6,
    LTR303_GAIN_96 = 7
}ltr303_gain_t;

typedef enum{
    LTR303_INTEGTIME_100,
    LTR303_INTEGTIME_50,
    LTR303_INTEGTIME_200,
    LTR303_INTEGTIME_400,
    LTR303_INTEGTIME_150,
    LTR303_INTEGTIME_250,
    LTR303_INTEGTIME_300,
    LTR303_INTEGTIME_350
}ltr303_intergrationTime_t;

esp_err_t i2c_master_init(void);
esp_err_t ltr303_register_write_byte(uint8_t reg_addr, uint8_t data);
esp_err_t ltr303_set_gain(ltr303_gain_t ltr303_gain);
esp_err_t ltr303_set_intergrationTime(ltr303_intergrationTime_t ltr303_intergrationTime);
esp_err_t ltr303_register_read(uint8_t reg_addr, uint32_t *data, size_t len);
float ltr303_value(uint16_t data_ch0, uint16_t data_ch1, ltr303_gain_t ltr303_gain, ltr303_intergrationTime_t ltr303_intergrationTime);
#endif