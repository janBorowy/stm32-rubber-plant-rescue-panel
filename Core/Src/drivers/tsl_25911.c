#include "tsl_25911.h"

#include "stm32f4xx_hal_def.h"
#include "stm32f4xx_hal_i2c.h"
#include <stdint.h>

#define SLAVE_ADDR 0x29

#define NORMAL_TRAN_FLAGS 0xA0
#define ENABLE_REGISTER_FLAGS 0x03
#define DISABLE_REGISTER_FLAGS 0x0

#define ENABLE_REGISTER_ADDR 0x00
#define CONTROL_REGISTER_ADDR 0x01
#define CHANNEL_0_ADDR 0x14
#define CHANNEL_1_ADDR 0x16

TslIntegrationTime _time;
TslGain _gain;

TslStatus set_register(uint8_t register_address, uint8_t value);
TslStatus read_channel(uint8_t channel_addr, uint16_t *p_result);
float get_integration_time_value();
float get_gain_value();

TslStatus tsl_25911_enable_sensor_default() {
    return tsl_25911_enable_sensor(TSL_GAIN_LOW, TSL_TIME_100);
}

TslStatus tsl_25911_enable_sensor(TslGain gain,
                                  TslIntegrationTime integration_time) {
    TslStatus status;
    status = set_register(ENABLE_REGISTER_ADDR, ENABLE_REGISTER_FLAGS);
    if (status != TSL_OK) {
        return TSL_ERROR;
    }
    uint8_t control_flags = (gain << 4) | integration_time;
    status = set_register(CONTROL_REGISTER_ADDR, control_flags);
    if (status != TSL_OK) {
        return TSL_ERROR;
    }
    _time = integration_time;
    _gain = gain;
    return status;
}

TslStatus tsl_25911_disable_sensor() {
    return set_register(ENABLE_REGISTER_ADDR, DISABLE_REGISTER_FLAGS);
}

TslStatus read_channel(uint8_t channel_addr, uint16_t *p_result) {
    HAL_StatusTypeDef status;
    uint8_t command = NORMAL_TRAN_FLAGS | channel_addr;

    status = HAL_I2C_Master_Transmit(&hi2c1, SLAVE_ADDR << 1, &command, 1,
                                     HAL_MAX_DELAY);
    if (status != HAL_OK) {
        return TSL_ERROR;
    }

    uint8_t received_values[2];
    status = HAL_I2C_Master_Receive(&hi2c1, SLAVE_ADDR << 1, received_values, 2,
                                    HAL_MAX_DELAY);
    if (status != HAL_OK) {
        return TSL_ERROR;
    }
    *p_result = ((uint16_t)received_values[1] << 8) + received_values[0];
    return TSL_OK;
}

uint16_t tsl_25911_read_raw_channel_0() {
    uint16_t result;
    TslStatus status;
    status = read_channel(CHANNEL_0_ADDR, &result);
    if (status != TSL_OK) {
        return -1;
    }
    return result;
}

uint16_t tsl_25911_read_raw_channel_1() {
    uint16_t result;
    TslStatus status;
    status = read_channel(CHANNEL_1_ADDR, &result);
    if (status != TSL_OK) {
        return -1;
    }
    return result;
}

// stolen from: https://github.com/adafruit/Adafruit_TSL2591_Library/blob/master/Adafruit_TSL2591.cpp
uint32_t tsl_25911_read_lux() {
    float time = get_integration_time_value(), gain = get_gain_value();
    float count_per_lux, lux;
    uint16_t ch0 = tsl_25911_read_raw_channel_0(),
             ch1 = tsl_25911_read_raw_channel_1();

    if (ch0 == 0xFFFF || ch1 == 0xFFFF) {
        return -1;
    }

    count_per_lux = time * gain / 408.0f;

    lux = ((float)ch0 - (float)ch1) * (1.0F - ((float)ch1 / (float)ch0)) / count_per_lux;

    return lux;
}

TslStatus set_register(uint8_t register_address, uint8_t value) {
    HAL_StatusTypeDef status;
    uint8_t command = NORMAL_TRAN_FLAGS | (register_address && 0x0F);
    uint8_t payload[2] = {command, value};

    status = HAL_I2C_Master_Transmit(&hi2c1, SLAVE_ADDR << 1, payload, 2,
                                     HAL_MAX_DELAY);
    if (status != HAL_OK) {
        return TSL_ERROR;
    }
    return TSL_OK;
}

float get_integration_time_value() {
    switch (_time) {
    case TSL_TIME_100:
        return 100.0f;
    case TSL_TIME_200:
        return 200.0f;
    case TSL_TIME_300:
        return 300.0f;
    case TSL_TIME_400:
        return 400.0f;
    case TSL_TIME_500:
        return 500.0f;
    case TSL_TIME_600:
        return 600.0F;
    default:
        return 100.0f;
    }
}

float get_gain_value() {
    switch (_gain) {
    case TSL_GAIN_LOW:
        return 1.0f;
    case TSL_GAIN_MEDIUM:
        return 25.0f;
    case TSL_GAIN_HIGH:
        return 428.0f;
    case TSL_GAIN_MAX:
        return 9876.0f;
    default:
        return 1.0f;
    }
}
