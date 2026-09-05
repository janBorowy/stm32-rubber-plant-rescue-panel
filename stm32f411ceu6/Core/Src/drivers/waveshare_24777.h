#ifndef WAVESHARE_24777_H
#define WAVESHARE_24777_H

#define DISPLAY_WIDTH 128
#define DISPLAY_HEIGHT 96

#include <stdint.h>

typedef enum {
    OLED_OK,
    OLED_SPI_ERROR
} OledStatus;

OledStatus oled_enable(uint8_t contrast);
void oled_draw(uint8_t *data);

#endif