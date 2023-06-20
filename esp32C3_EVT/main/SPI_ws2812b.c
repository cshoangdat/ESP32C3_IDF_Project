#include "SPI_ws2812b.h"
#include "stdio.h"
#include "stdint.h"
spi_device_handle_t spi;
esp_err_t ret;
spi_transaction_t t;
static bool is_init_done = false;

void initSPI(void){
    ESP_LOGI(TAG_LED, "Inittializing SPI");
    spi_bus_config_t buscfg ={
        .mosi_io_num = MOSI_PIN,
        .miso_io_num = -1,
        .sclk_io_num = -1,
        .quadhd_io_num = -1,
        .quadwp_io_num = -1,
        .max_transfer_sz = 4092
    };
    ret = spi_bus_initialize(SPI2_HOST,&buscfg, SPI_DMA_DISABLED);
    ESP_ERROR_CHECK(ret);
    spi_device_interface_config_t devivecfg = {
        .command_bits = 0,
        .address_bits =0,
        .dummy_bits = 0,
        .clock_speed_hz = 5700000, //5500000,5700000
        .duty_cycle_pos = 128,
        .mode = 0,
        .spics_io_num = -1,
        .queue_size = 4
   };
    ret = spi_bus_add_device(SPI2_HOST, &devivecfg, &spi);
    int khz;
    khz = spi_get_actual_clock(APB_CLK_FREQ, 5700000, 128);
    ESP_ERROR_CHECK(ret);
    ESP_LOGI(TAG_LED, "spi_get_actual_clock %d", khz);
    ESP_LOGI(TAG_LED, "Complete Inittializing SPI");
    is_init_done = true;
}

void sendData_8bit(unsigned char data){
    memset(&t,0,sizeof(t));
    t.length = 8;
    t.tx_buffer = &data;
    ret = spi_device_transmit(spi, &t);
    ESP_LOGI(TAG_LED, "Du lieu gui: %d", data);
    ESP_ERROR_CHECK(ret);
}

void sendData_24byte(unsigned char data){
    memset(&t,0,sizeof(t));
    t.length = 8*24;
    t.tx_buffer = data;
    spi_device_transmit(spi, &t);
}

void ws2812b_pixel(unsigned char  red, unsigned char  green, unsigned char  blue){
    unsigned char  maxBit = 24;
    unsigned long value = 0x00000000;
    value = (((unsigned long)green << 16) | ((unsigned long) red <<8) | ((unsigned long)blue));
    unsigned char arr[24];
    while(maxBit > 0){
        if((value & 0x800000)!=0){
            // sendData_8bit(bit_one);
            arr[24-maxBit] = bit_one;
            // ESP_LOGI(TAG_LED, "send bit 1");
        }
        else{
            // sendData_8bit(bit_zero);
            arr[24-maxBit] = bit_zero;
            // ESP_LOGI(TAG_LED, "send bit 0");
        }
        value <<= 1;
        maxBit--;
    }
    // printf("\nGia tri trong mang arr:\n");
    // for(int i =0; i<24;i++){
    //     printf("%d ",arr[i]);
    // }
    // printf("\n");
    
    sendDataReset();

    memset(&t,0,sizeof(t));
    t.length = 8*24;
    t.tx_buffer = arr;
    spi_device_transmit(spi, &t);

    sendDataReset();
}

void Test(void){
    uint8_t data[72]={  bit_zero,bit_zero,bit_zero,bit_zero,bit_zero,bit_zero,bit_zero,bit_zero,
                        bit_zero,bit_zero,bit_zero,bit_zero,bit_zero,bit_zero,bit_zero,bit_zero,
                        bit_zero,bit_zero,bit_zero,bit_zero,bit_zero,bit_zero,bit_zero,bit_zero,
                        bit_zero,bit_zero,bit_zero,bit_zero,bit_zero,bit_zero,bit_zero,bit_zero,
                        bit_zero,bit_zero,bit_zero,bit_zero,bit_zero,bit_zero,bit_zero,bit_zero,
                        bit_zero,bit_zero,bit_zero,bit_zero,bit_zero,bit_zero,bit_zero,bit_zero,
                        bit_zero,bit_zero,bit_zero,bit_zero,bit_zero,bit_zero,bit_zero,bit_zero,
                        bit_zero,bit_zero,bit_zero,bit_zero,bit_zero,bit_zero,bit_zero,bit_one,
                        bit_zero,bit_zero,bit_zero,bit_zero,bit_zero,bit_zero,bit_zero,bit_zero};

    uint8_t data1[25]={ bit_zero, bit_zero,bit_zero,bit_zero,bit_zero,bit_zero,bit_zero,bit_zero,bit_zero,
                    bit_zero,bit_zero,bit_zero,bit_zero,bit_zero,bit_zero,bit_zero,bit_zero, // G
                    bit_zero,bit_zero,bit_zero,bit_zero,bit_zero,bit_zero,bit_zero,bit_one};

    uint8_t reset[8]={bit_zero,bit_zero,bit_zero,bit_zero,bit_zero,bit_zero,bit_zero,bit_zero};
    spi_transaction_t t;

    t.length = 8*25;
    t.tx_buffer = data1;
    ret = spi_device_transmit(spi, &t);

    t.length = 8*40;
    t.tx_buffer = 0;
    ret = spi_device_transmit(spi, &t);
}

void sendDataReset(void){
    uint8_t reset[60] = {0};
    memset(&t,0,sizeof(t));
    t.length = 8*60;
    t.tx_buffer = reset;
    spi_device_transmit(spi, &t);
}

bool is_init_SPI_done(){
    return is_init_done;
}