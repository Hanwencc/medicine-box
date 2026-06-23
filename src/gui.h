#pragma once

#include <lvgl.h>

extern lv_obj_t* ui_main_screen;
extern lv_obj_t* ui_tileview;
extern lv_obj_t* ui_tile_main;
extern lv_obj_t* ui_tile_settings;

extern lv_obj_t* label_time;
extern lv_obj_t* label_next_time;
extern lv_obj_t* label_countdown;
extern lv_obj_t* label_battery;
extern lv_obj_t* label_battery_icon;
extern lv_obj_t* label_wifi;

void gui_init(void);
void gui_update_time(const char* time_str);
void gui_update_next_time(const char* next_time_str);
void gui_update_countdown(const char* countdown_str);
void gui_update_battery(int level);
void gui_update_wifi(bool connected);
