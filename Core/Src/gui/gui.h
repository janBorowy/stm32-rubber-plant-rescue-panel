#ifndef GUI_H
#define GUI_H

#include <stdint.h>
#include <sys/_types.h>
#include "../drivers/waveshare_24777.h"

#define CHAR_WIDTH 6
#define CHAR_HEIGHT 8

typedef enum {
    BLACK = 0,
    GRAY = 1,
    WHITE = 15
} Shade;

void gui_refresh();
void gui_clear_display(Shade shade);
void gui_draw_line(uint8_t start_x, uint8_t start_y, uint8_t end_x, uint8_t end_y, Shade shade);
void gui_draw_str(char *c, uint8_t start_x, uint8_t start_y, Shade shade);

#endif