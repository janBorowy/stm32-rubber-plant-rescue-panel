#ifndef SENSOR_STATS_H
#define SENSOR_STATS_H

#include <stdint.h>

typedef struct {
    uint32_t lux;
    float temp;
    float moist;
} SensorStats;

void get_stats(SensorStats *stats);

#endif