#include "stats_gui.h"

#include "stats.h"
#include <limits.h>
#include <stdio.h>
#include <string.h>

#include "../gui/gui.h"
#include "plant_characteristics.h"

#define MAX_TEXT_LEN ((DISPLAY_WIDTH)/(CHAR_WIDTH))
char text_buff[MAX_TEXT_LEN];

void draw_header();
void draw_stats(SensorStats *stats);
void buffer_temp_val_str(float temp);
void buffer_moist_val_str(float moist);

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
    gui_draw_str(text_buff, 0, 15, WHITE);
    buffer_temp_val_str(stats->temp);
    gui_draw_str(text_buff, 0, 27, WHITE);
    buffer_moist_val_str(stats->moist);
    gui_draw_str(text_buff, 0, 39, WHITE);
}

void buffer_temp_val_str(float temp) {
    if (temp < PLANT_TEMP_COLD) {
        sprintf(text_buff, "Temp: %.2f - COLD!", temp);
        return;
    } else if (temp > PLANT_TEMP_HOT) {
        sprintf(text_buff, "Temp: %.2f - HOT!", temp);
        return;
    }
    sprintf(text_buff, "Temp: %.2f - good", temp);
    return;
}

void buffer_moist_val_str(float moist) {
    if (moist < PLANT_MOIST_DRY) {
        sprintf(text_buff, "Moist: %.2f%% - DRY!", moist);
        return;
    } else if (moist > PLANT_MOIST_HIGH) {
        sprintf(text_buff, "Moist: %.2f%% - HIGH!", moist);
        return;
    }
        sprintf(text_buff, "Moist: %.2f%% - good", moist);
    return;
}