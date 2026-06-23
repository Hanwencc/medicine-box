#include "gui.h"
#include <cstdio>

lv_obj_t* ui_main_screen;
lv_obj_t* ui_tileview;
lv_obj_t* ui_tile_main;
lv_obj_t* ui_tile_settings;

lv_obj_t* label_time;
lv_obj_t* label_next_time;
lv_obj_t* label_countdown;
lv_obj_t* label_battery;

static void next_time_click_event_cb(lv_event_t * e) {
    lv_event_code_t code = lv_event_get_code(e);
    if(code == LV_EVENT_CLICKED) {
        // Go to settings tile
        lv_obj_set_tile(ui_tileview, ui_tile_settings, LV_ANIM_ON);
    }
}

void gui_init(void) {
    ui_main_screen = lv_scr_act();
    
    // Create tileview for swiping between pages
    ui_tileview = lv_tileview_create(ui_main_screen);
    lv_obj_set_size(ui_tileview, LV_PCT(100), LV_PCT(100));
    lv_obj_set_scrollbar_mode(ui_tileview, LV_SCROLLBAR_MODE_OFF);

    // Tile 1: Main Page
    ui_tile_main = lv_tileview_add_tile(ui_tileview, 0, 0, LV_DIR_LEFT | LV_DIR_RIGHT);
    
    // Tile 2: Settings Page
    ui_tile_settings = lv_tileview_add_tile(ui_tileview, 1, 0, LV_DIR_LEFT | LV_DIR_RIGHT);

    // --- Main Page UI ---
    
    // Battery
    label_battery = lv_label_create(ui_tile_main);
    lv_obj_align(label_battery, LV_ALIGN_TOP_RIGHT, -10, 10);
    lv_label_set_text(label_battery, "Bat: 100%");

    // Current Time
    label_time = lv_label_create(ui_tile_main);
    lv_obj_set_style_text_font(label_time, &lv_font_montserrat_28, 0);
    lv_obj_align(label_time, LV_ALIGN_TOP_MID, 0, 40);
    lv_label_set_text(label_time, "12:00:00");

    // Countdown
    label_countdown = lv_label_create(ui_tile_main);
    lv_obj_set_style_text_font(label_countdown, &lv_font_montserrat_16, 0);
    lv_obj_align(label_countdown, LV_ALIGN_CENTER, 0, 10);
    lv_label_set_text(label_countdown, "Left: 01:23:45");

    // Next Pill Time Area (Clickable)
    lv_obj_t* btn_next_time = lv_btn_create(ui_tile_main);
    lv_obj_align(btn_next_time, LV_ALIGN_BOTTOM_MID, 0, -20);
    lv_obj_set_size(btn_next_time, LV_PCT(90), 50);
    lv_obj_add_event_cb(btn_next_time, next_time_click_event_cb, LV_EVENT_CLICKED, NULL);

    label_next_time = lv_label_create(btn_next_time);
    lv_obj_center(label_next_time);
    lv_label_set_text(label_next_time, "Next: 13:30");


    // --- Settings Page UI ---
    lv_obj_t* label_settings_title = lv_label_create(ui_tile_settings);
    lv_obj_align(label_settings_title, LV_ALIGN_TOP_MID, 0, 20);
    lv_label_set_text(label_settings_title, "Settings Page");

    lv_obj_t* label_settings_info = lv_label_create(ui_tile_settings);
    lv_obj_align(label_settings_info, LV_ALIGN_CENTER, 0, 0);
    lv_label_set_text(label_settings_info, "Swipe Left to return");
}

void gui_update_time(const char* time_str) {
    if(label_time) lv_label_set_text(label_time, time_str);
}

void gui_update_next_time(const char* next_time_str) {
    if(label_next_time) {
        char buf[32];
        snprintf(buf, sizeof(buf), "Next: %s", next_time_str);
        lv_label_set_text(label_next_time, buf);
    }
}

void gui_update_countdown(const char* countdown_str) {
    if(label_countdown) lv_label_set_text(label_countdown, countdown_str);
}

void gui_update_battery(int level) {
    if(label_battery) {
        char buf[16];
        snprintf(buf, sizeof(buf), "Bat: %d%%", level);
        lv_label_set_text(label_battery, buf);
    }
}
