#include "gui.h"
#include <math.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "zxpix_font.h"

#define MIN(a, b) (((a) <= (b)) ? (a) : (b))
#define MAX(a, b) (((a) >= (b)) ? (a) : (b))

_Bool Needs_Refresh;
uint8_t Gui_Display_Data[DISPLAY_WIDTH * DISPLAY_HEIGHT];

void gui_refresh() {
    if (Needs_Refresh) {
        oled_draw(Gui_Display_Data);
        Needs_Refresh = false;
    }
}

void set_pixel(uint8_t x, uint8_t y, Shade shade) {
    if (x >= 0 && x < DISPLAY_WIDTH &&
        y >= 0 && y < DISPLAY_HEIGHT) {
        Gui_Display_Data[y * DISPLAY_WIDTH + x] = shade;
        Needs_Refresh = true;
    }
}

void gui_clear_display(Shade shade) {
    memset(Gui_Display_Data, shade, DISPLAY_WIDTH * DISPLAY_HEIGHT);
    Needs_Refresh = true;
}

void gui_draw_line(uint8_t start_x, uint8_t start_y, uint8_t end_x, uint8_t end_y, Shade shade) {
    int x_diff = end_x - start_x;
    int y_diff = end_y - start_y;
    float step;
    if (abs(x_diff) >= abs(y_diff)) {
        step = abs(x_diff);
    } else {
        step = abs(y_diff);
    }

    float dx = x_diff / step, dy = y_diff / step;
    float x = start_x, y = start_y;
    int i = 0;
    while (i <= step) {
        set_pixel(round(x), round(y), shade);
        x = x + dx;
        y = y + dy;
        i++;
    }
}

void gui_draw_char(unsigned char c, uint8_t start_x, uint8_t start_y, Shade shade) {
    int char_idx = c - 32;
    if (char_idx < 0 || char_idx > 94) {
        char_idx = 0;
    }

    const uint8_t *bitmap = font[char_idx]; 
    for (int col = 0; col < CHAR_WIDTH; col++) {
        for (int row = 0; row < CHAR_HEIGHT; row++) {
            if ((bitmap[col] >> row) & 1) {
                set_pixel(start_x + col, start_y + row, shade);
            } 
        }
    }
}

void gui_draw_str(char *c, uint8_t start_x, uint8_t start_y, Shade shade) {
    uint8_t x = start_x;
    while(*c) {
        gui_draw_char(*c++, x, start_y, shade);
        x += CHAR_WIDTH;
    }
}

void gui_draw_leaf(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1, uint8_t half_width, Shade shade) {
    float dx = (float)x1 - (float)x0;
    float dy = (float)y1 - (float)y0;
    float len = sqrtf(dx * dx + dy * dy);
    if (len < 1.0f) {
        len = 1.0f;
    }
    float ux = dx / len, uy = dy / len;
    float px = -uy, py = ux;

    int steps = (int)len;
    for (int t = 0; t <= steps; t++) {
        float frac = (float)t / (float)steps;
        float taper = sinf(frac * M_PI);
        float w = half_width * taper;

        float cx = x0 + ux * t;
        float cy = y0 + uy * t;
        for (float o = -w; o <= w; o += 0.5f) {
            set_pixel((uint8_t)roundf(cx + px * o), (uint8_t)roundf(cy + py * o), shade);
        }
    }
}

void gui_draw_plant(uint8_t base_x, uint8_t base_y, uint8_t size, Shade shade) {
    int cx = base_x;
    int base = base_y;

    int pot_h = size / 2;
    int pot_top_w = size;
    int pot_bot_w = (size * 3) / 4;
    int pot_top_y = base - pot_h;

    gui_draw_line(cx - pot_top_w / 2, pot_top_y, cx - pot_bot_w / 2, base, shade);
    gui_draw_line(cx + pot_top_w / 2, pot_top_y, cx + pot_bot_w / 2, base, shade);
    gui_draw_line(cx - pot_top_w / 2, pot_top_y, cx + pot_top_w / 2, pot_top_y, shade);
    gui_draw_line(cx - pot_bot_w / 2, base, cx + pot_bot_w / 2, base, shade);

    int stem_h = size;
    int stem_top_y = pot_top_y - stem_h;
    gui_draw_line(cx, pot_top_y, cx, stem_top_y, shade);

    int leaf_len = (size * 2) / 3;
    int leaf_width = MAX(2, size / 7);
    for (int i = 0; i < 3; i++) {
        int ly = stem_top_y + i * (stem_h / 3);
        gui_draw_leaf(cx, ly, cx - leaf_len, ly - leaf_len / 2, leaf_width, shade);
        gui_draw_leaf(cx, ly, cx + leaf_len, ly - leaf_len / 2, leaf_width, shade);
    }
}