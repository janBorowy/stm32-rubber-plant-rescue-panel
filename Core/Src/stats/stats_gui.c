#include "stats_gui.h"

#include "../gui/gui.h"
#include "stats.h"
#include <limits.h>
#include <stdio.h>
#include <string.h>

#define MAX_TEXT_LEN ((DISPLAY_WIDTH)/(CHAR_WIDTH))
char text_buff[MAX_TEXT_LEN];

void draw_header();
void draw_stats(SensorStats *stats);

void stats_gui_refresh(SensorStats *stats) {

    gui_clear_display(BLACK);
    draw_header();
    draw_stats(stats);

    gui_refresh();
}

void draw_header() {
    strcpy(text_buff, "Plant info");
    int padding = (MAX_TEXT_LEN - strlen(text_buff)) / 2;
    gui_draw_str("Plant info", padding * CHAR_WIDTH, 1, WHITE);
    gui_draw_line(0, 10, DISPLAY_WIDTH - 1, 10, WHITE);
}

void draw_stats(SensorStats *stats) {
    sprintf(text_buff, "Lux: %lu", stats->lux);
    gui_draw_str(text_buff, 6, 40, WHITE);
    sprintf(text_buff, "Temp: %.2f", stats->temp);
    gui_draw_str(text_buff, 6, 48, WHITE);
    sprintf(text_buff, "Moist: %.2f%%", stats->moist);
    gui_draw_str(text_buff, 6, 56, WHITE);
}