#include "stats.h"
#include "../drivers/tsl_25911.h"
#include "stm32f4xx_hal_adc.h"
#include "stm32f4xx_hal_dma.h"
#include <stdint.h>
#include <stdbool.h>

#define MAX_MOIST_VOLT (3000)
#define MIN_MOIST_VOLT (1350)

extern ADC_HandleTypeDef hadc1;

_Bool Conv_Complete = false;

float calculate_temp(uint16_t raw_value);
float calculate_moist(uint16_t raw_value);
float clamp(float val, float min, float max);

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef *hadc) {
    if (hadc->Instance == ADC1) {
        Conv_Complete = true;
    }
}

void get_stats(SensorStats *stats) {
    stats->lux = tsl_25911_read_lux();

    uint16_t captured_values[2];
    HAL_ADC_Start_DMA(&hadc1, (uint32_t *) captured_values, 2);
    while (!Conv_Complete); 
    HAL_ADC_Stop_DMA(&hadc1);
    Conv_Complete = false;
    stats->temp = calculate_temp(captured_values[0]);
    stats->moist = calculate_moist(captured_values[1]);
}

float calculate_temp(uint16_t raw_value) {    
    float m_volts, temp;
    m_volts = ((float) raw_value) / 4095 * 3300;
    temp = ((m_volts - 750.0) / 10) + 25;
    return temp;
}

float calculate_moist(uint16_t raw_value) {
    float m_volts, moist;
    m_volts = ((float) raw_value) / 4095 * 3300;
    moist = (1.0 - (m_volts - MIN_MOIST_VOLT) / (MAX_MOIST_VOLT - MIN_MOIST_VOLT)) * 100;
    return clamp(moist, 0.0, 100.0);
}

float clamp(float val, float min, float max) {
    if (val < min) {
        return min;
    } else if (val > max) {
        return max;
    }
    return val;
}