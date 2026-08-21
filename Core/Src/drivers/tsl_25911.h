#ifndef TSL_25911_H
#define TSL_25911_H
#include "stm32f4xx_hal.h"
#include <stdint.h>

typedef enum {
    TSL_OK,
    TSL_ERROR
} TslStatus;

typedef enum {
    TSL_GAIN_LOW = 0b00,
    TSL_GAIN_MEDIUM = 0b01,
    TSL_GAIN_HIGH = 0b10,
    TSL_GAIN_MAX = 0b11
} TslGain;

typedef enum {
    TSL_TIME_100 = 0b000,
    TSL_TIME_200 = 0b001,
    TSL_TIME_300 = 0b010,
    TSL_TIME_400 = 0b011,
    TSL_TIME_500 = 0b100,
    TSL_TIME_600 = 0b101
} TslIntegrationTime;

typedef struct {

} TslHandle;

extern I2C_HandleTypeDef hi2c1;

/*
    This module uses only i2c polling mode, data exchanged is 2-byte long at most.
*/

TslStatus tsl_25911_enable_sensor_default();
TslStatus tsl_25911_enable_sensor(TslGain gain, TslIntegrationTime integration_time);
TslStatus tsl_25911_disable_sensor();

/* Requires that sensor is enabled by tsl_25911_enable_sensor* */
uint32_t tsl_25911_read_lux();

#endif