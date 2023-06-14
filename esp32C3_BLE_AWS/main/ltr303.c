#include "ltr303.h"

esp_err_t i2c_master_init(void){
    uint8_t i2c_master_port = I2C_MASTER_NUM;
    static const i2c_config_t conf = {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = I2C_MASTER_SDA_PIN,
        .scl_io_num = I2C_MASTER_SCL_PIN,
        .sda_pullup_en = GPIO_PULLUP_ENABLE,
        .scl_pullup_en = GPIO_PULLUP_ENABLE,
        .master.clk_speed  = I2C_MASTER_FREQ_HZ
    };
    i2c_param_config(i2c_master_port, &conf);
    return i2c_driver_install(i2c_master_port, conf.mode, I2C_MASTER_RX_BUF_DISABLE, I2C_MASTER_TX_BUF_DISABLE, 0);
}

esp_err_t ltr303_register_write_byte(uint8_t reg_addr, uint8_t data){
    uint8_t write_buf[2] = {reg_addr, data};
    return i2c_master_write_to_device(I2C_MASTER_NUM, LTR303_I2C_ADRESS, write_buf, sizeof(write_buf), I2C_MASTER_TIMEOUT/portTICK_PERIOD_MS);
}

esp_err_t ltr303_set_gain(ltr303_gain_t ltr303_gain){
    return ltr303_register_write_byte(LTR303_ALS_CTRL, ltr303_gain);
}

esp_err_t ltr303_set_intergrationTime(ltr303_intergrationTime_t ltr303_intergrationTime){
    return ltr303_register_write_byte(LTR303_MEAS_RATE, ltr303_intergrationTime);
}

esp_err_t ltr303_register_read(uint8_t reg_addr, uint32_t *data, size_t len){
    return i2c_master_write_read_device(I2C_MASTER_NUM, LTR303_I2C_ADRESS, &reg_addr, 1, data, len, I2C_MASTER_TIMEOUT/portTICK_PERIOD_MS);
}

float ltr303_value(uint16_t data_ch0, uint16_t data_ch1, ltr303_gain_t ltr303_gain, ltr303_intergrationTime_t ltr303_intergrationTime){
    float result = 0;
    float ratio = (float)data_ch1/(data_ch0 + data_ch1);
    if(ratio < 0.45) result = (float)(1.7743*data_ch0 + 1.1059*data_ch1)/ltr303_gain/ltr303_intergrationTime;
    else if(ratio >=0.45 && ratio < 0.64) result = (float)(4.2785*data_ch0 - 1.9548*data_ch1)/ltr303_gain/ltr303_intergrationTime;
    else if(ratio >= 0.64 && ratio < 0.85) result = (float)(0.5926*data_ch0 + 0.1185*data_ch1)/ltr303_gain/ltr303_intergrationTime;
    else result = 0;
    return result;
}


