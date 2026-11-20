#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"

#define MPU6050_ADDR 0x69 // AD0 pin pulled HIGH (3.3V) to avoid conflict with DS3231

// MPU6050 Registers
#define MPU_REG_PWR_MGMT_1 0x6B
#define MPU_REG_ACCEL_XOUT_H 0x3B

struct mpu6050_data {
    float accel[3]; // X, Y, Z
    float gyro[3];  // X, Y, Z
    float temp;    // Temperature in degrees Celsius
} mpuValues;


class mpu6050 {

    public:


        static void init(i2c_inst_t* i2c_port, uint8_t addr = MPU6050_ADDR) {
            my_i2c_port = i2c_port;
            my_addr = addr;
            is_initialized = true;
            // Wake up MPU6050 (clear sleep bit in PWR_MGMT_1)
            uint8_t buf[2] = {MPU_REG_PWR_MGMT_1, 0x00};
            i2c_write_blocking(i2c_port, MPU6050_ADDR, buf, 2, false);
        };

        static mpu6050_data read_values() { 
            if (!is_initialized) {
                printf("MPU6050 not initialized. Call init() first.\n");
                return mpu6050_data{-1, -1, -1, -1, -1, -1, -1}; // Return invalid data
            }
            uint8_t buffer[14]; 
            int16_t accel[3], gyro[3], temp_raw;
            uint8_t reg = MPU_REG_ACCEL_XOUT_H;

            // Point to ACCEL_XOUT_H register and read 14 bytes (Accel, Temp, Gyro)
            i2c_write_blocking(my_i2c_port, my_addr, &reg, 1, true);
            i2c_read_blocking(my_i2c_port, my_addr, buffer, 14, false);

            accel[0] = (buffer[0] << 8) | buffer[1];
            accel[1] = (buffer[2] << 8) | buffer[3];
            accel[2] = (buffer[4] << 8) | buffer[5];
            // buffer[6..7] is internal temperature
            temp_raw = (buffer[6] << 8) | buffer[7];
            gyro[0]  = (buffer[8] << 8) | buffer[9];
            gyro[1]  = (buffer[10] << 8) | buffer[11];
            gyro[2]  = (buffer[12] << 8) | buffer[13];

            return mpu6050_data{
                .accel = {accel[0] / 16384.0f, accel[1] / 16384.0f, accel[2] / 16384.0f},
                .gyro = {gyro[0] / 131.0f, gyro[1] / 131.0f, gyro[2] / 131.0f},
                .temp = temp_raw / 340.0f + 36.25f
            };
        }

        private:
        static inline i2c_inst_t* my_i2c_port;
        static inline uint8_t my_addr;
        static inline bool is_initialized;
};
