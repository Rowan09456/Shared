#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"

#define DS3231_ADDR 0x68

struct ds3231_dateTime {
    uint8_t sec;
    uint8_t min;
    uint8_t hour;
    uint8_t day_of_week;
    uint8_t date;
    uint8_t month;
    uint8_t year;
};

class ds3231 {
    public:
    static void init(i2c_inst_t* i2c_port, uint8_t addr = DS3231_ADDR) {
        my_i2c_port = i2c_port;
        my_addr = addr;
        is_initialized = true;
    }

    static int set_time(ds3231_dateTime dt) {
        if (!is_initialized) {
            printf("DS3231 not initialized. Call initialize() first.\n");
            return -1; // Return error code
        }
        uint8_t buf[8] = {
            0x00, // Start at register 0x00
            dec_to_bcd(dt.sec),
            dec_to_bcd(dt.min),
            dec_to_bcd(dt.hour),
            dec_to_bcd(dt.day_of_week),
            dec_to_bcd(dt.date),
            dec_to_bcd(dt.month),
            dec_to_bcd(dt.year)
        };
        i2c_write_blocking(my_i2c_port, my_addr, buf, 8, false);
        return 0; // Success
    }

    static ds3231_dateTime read_time() {
        if (!is_initialized) {
            printf("DS3231 not initialized. Call initialize() first.\n");
            return ds3231_dateTime{0, 0, 0, 0, 0, 0, 0}; // Return default dateTime
        }
        uint8_t reg = 0x00;
        uint8_t buf[7];

        i2c_write_blocking(my_i2c_port, my_addr, &reg, 1, true);
        i2c_read_blocking(my_i2c_port, my_addr, buf, 7, false);

        ds3231_dateTime dt;
        dt.sec   = bcd_to_dec(buf[0] & 0x7F);
        dt.min   = bcd_to_dec(buf[1]);
        dt.hour  = bcd_to_dec(buf[2] & 0x3F); // 24-hour mode
        dt.day_of_week = bcd_to_dec(buf[3]);
        dt.date  = bcd_to_dec(buf[4]);
        dt.month = bcd_to_dec(buf[5] & 0x1F);
        dt.year  = bcd_to_dec(buf[6]);

        return dt;
    }
  

    private:
    static inline i2c_inst_t* my_i2c_port;
    static inline uint8_t my_addr;
    static inline bool is_initialized;

    static uint8_t dec_to_bcd(uint8_t val) {
        return ((val / 10) << 4) | (val % 10);
    }
    static uint8_t bcd_to_dec(uint8_t val) {
        return ((val >> 4) * 10) + (val & 0x0F);
    }

};