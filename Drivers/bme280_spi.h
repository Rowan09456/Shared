/*
GPIO 10 (pin 14)-> SCK on BME-280 board
GPIO 11 (pin 15)-> SDA on BME280 board
GPIO 12 (pin 16)-> SDO on BME280 board
GPIO 13 (pin 17)-> CSN on BME280 board
*/

#define mySPI spi1
#define mySPI_SCK 10
#define mySPI_TX 11
#define mySPI_RX 12
#define mySPI_CSN 13

void write_register(uint8_t reg, uint8_t data);
void read_registers(uint8_t reg, uint8_t *buf, uint16_t len);
void read_compensation_parameters();
void bme280_read_raw(int32_t *humidity, int32_t *pressure, int32_t *temperature);
uint32_t compensate_humidity(int32_t adc_H);
uint32_t compensate_pressure(int32_t adc_P);
int32_t compensate_temp(int32_t adc_T);