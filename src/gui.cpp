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
lv_obj_t* label_battery_icon;
lv_obj_t* label_wifi;

static void set_cjk_font(lv_obj_t* obj) {
    lv_obj_set_style_text_font(obj, &lv_font_simsun_16_cjk, 0);
}

static const char* battery_symbol_for_level(int level) {
    if(level >= 90) return LV_SYMBOL_BATTERY_FULL;
    if(level >= 65) return LV_SYMBOL_BATTERY_3;
    if(level >= 35) return LV_SYMBOL_BATTERY_2;
    if(level >= 10) return LV_SYMBOL_BATTERY_1;
    return LV_SYMBOL_BATTERY_EMPTY;
}

static void next_time_click_event_cb(lv_event_t * e) {
    lv_event_code_t code = lv_event_get_code(e);
    if(code == LV_EVENT_CLICKED) {
        // Go to settings tile
        lv_obj_set_tile(ui_tileview, ui_tile_settings, LV_ANIM_ON);
    }
}

void gui_init(void) {
    ui_main_screen = lv_scr_act();
    lv_obj_set_style_bg_color(ui_main_screen, lv_color_hex(0x101418), 0);
    
    // Create tileview for swiping between pages
    ui_tileview = lv_tileview_create(ui_main_screen);
    lv_obj_set_size(ui_tileview, LV_PCT(100), LV_PCT(100));
    lv_obj_set_scrollbar_mode(ui_tileview, LV_SCROLLBAR_MODE_OFF);
    lv_obj_set_style_bg_color(ui_tileview, lv_color_hex(0x101418), 0);
    lv_obj_set_style_border_width(ui_tileview, 0, 0);
    lv_obj_set_style_pad_all(ui_tileview, 0, 0);

    // Tile 1: Main Page
    ui_tile_main = lv_tileview_add_tile(ui_tileview, 0, 0, LV_DIR_LEFT | LV_DIR_RIGHT);
    lv_obj_set_style_bg_color(ui_tile_main, lv_color_hex(0x101418), 0);
    lv_obj_set_style_pad_all(ui_tile_main, 12, 0);
    
    // Tile 2: Settings Page
    ui_tile_settings = lv_tileview_add_tile(ui_tileview, 1, 0, LV_DIR_LEFT | LV_DIR_RIGHT);
    lv_obj_set_style_bg_color(ui_tile_settings, lv_color_hex(0x101418), 0);
    lv_obj_set_style_pad_all(ui_tile_settings, 12, 0);

    // --- Top status overlay ---
    label_wifi = lv_label_create(ui_main_screen);
    set_cjk_font(label_wifi);
    lv_obj_set_style_text_color(label_wifi, lv_color_hex(0x59636C), 0);
    lv_obj_align(label_wifi, LV_ALIGN_TOP_LEFT, 10, 8);
    lv_label_set_text(label_wifi, LV_SYMBOL_WIFI);

    label_battery_icon = lv_label_create(ui_main_screen);
    set_cjk_font(label_battery_icon);
    lv_obj_set_style_text_color(label_battery_icon, lv_color_hex(0xD9F0E4), 0);
    lv_obj_align(label_battery_icon, LV_ALIGN_TOP_RIGHT, -64, 8);
    lv_label_set_text(label_battery_icon, LV_SYMBOL_BATTERY_FULL);

    label_battery = lv_label_create(ui_main_screen);
    lv_obj_set_style_text_font(label_battery, &lv_font_montserrat_16, 0);
    lv_obj_set_style_text_color(label_battery, lv_color_hex(0xD9F0E4), 0);
    lv_obj_align(label_battery, LV_ALIGN_TOP_RIGHT, -6, 8);
    lv_label_set_text(label_battery, "100%");

    // --- Main Page UI ---
    lv_obj_t* label_title = lv_label_create(ui_tile_main);
    set_cjk_font(label_title);
    lv_obj_set_style_text_color(label_title, lv_color_hex(0x9AA7B2), 0);
    lv_obj_align(label_title, LV_ALIGN_TOP_MID, 0, 24);
    lv_label_set_text(label_title, "智能服用");

    // Current Time
    label_time = lv_label_create(ui_tile_main);
    lv_obj_set_style_text_font(label_time, &lv_font_montserrat_28, 0);
    lv_obj_set_style_text_color(label_time, lv_color_hex(0xF5FAF7), 0);
    lv_obj_align(label_time, LV_ALIGN_TOP_MID, 0, 54);
    lv_label_set_text(label_time, "12:00:00");

    // Countdown
    label_countdown = lv_label_create(ui_tile_main);
    set_cjk_font(label_countdown);
    lv_obj_set_style_text_color(label_countdown, lv_color_hex(0xDCE7EF), 0);
    lv_obj_set_width(label_countdown, 150);
    lv_obj_set_style_text_align(label_countdown, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_align(label_countdown, LV_ALIGN_CENTER, 0, -8);
    lv_label_set_text(label_countdown, "下次服用\n01:23:45");

    // Next Pill Time Area (Clickable)
    lv_obj_t* btn_next_time = lv_btn_create(ui_tile_main);
    lv_obj_align(btn_next_time, LV_ALIGN_BOTTOM_MID, 0, -20);
    lv_obj_set_size(btn_next_time, 150, 54);
    lv_obj_set_style_radius(btn_next_time, 8, 0);
    lv_obj_set_style_bg_color(btn_next_time, lv_color_hex(0x1F7A5A), 0);
    lv_obj_set_style_shadow_width(btn_next_time, 0, 0);
    lv_obj_add_event_cb(btn_next_time, next_time_click_event_cb, LV_EVENT_CLICKED, NULL);

    label_next_time = lv_label_create(btn_next_time);
    set_cjk_font(label_next_time);
    lv_obj_set_style_text_color(label_next_time, lv_color_hex(0xFFFFFF), 0);
    lv_obj_center(label_next_time);
    lv_label_set_text(label_next_time, "下次 13:30");


    // --- Settings Page UI ---
    lv_obj_t* label_settings_title = lv_label_create(ui_tile_settings);
    set_cjk_font(label_settings_title);
    lv_obj_set_style_text_color(label_settings_title, lv_color_hex(0xF5FAF7), 0);
    lv_obj_align(label_settings_title, LV_ALIGN_TOP_MID, 0, 20);
    lv_label_set_text(label_settings_title, "设置");

    lv_obj_t* label_settings_info = lv_label_create(ui_tile_settings);
    set_cjk_font(label_settings_info);
    lv_obj_set_style_text_color(label_settings_info, lv_color_hex(0xB8C6D1), 0);
    lv_obj_set_width(label_settings_info, 150);
    lv_obj_set_style_text_align(label_settings_info, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_align(label_settings_info, LV_ALIGN_CENTER, 0, 0);
    lv_label_set_text(label_settings_info, "向右滑动返回\n更多设置待添加");

    gui_update_wifi(false);
}

void gui_update_time(const char* time_str) {
    if(label_time) lv_label_set_text(label_time, time_str);
}

void gui_update_next_time(const char* next_time_str) {
    if(label_next_time) {
        char buf[32];
        snprintf(buf, sizeof(buf), "下次 %s", next_time_str);
        lv_label_set_text(label_next_time, buf);
    }
}

void gui_update_countdown(const char* countdown_str) {
    if(label_countdown) lv_label_set_text(label_countdown, countdown_str);
}

void gui_update_battery(int level) {
    if(label_battery) {
        char buf[16];
        snprintf(buf, sizeof(buf), "%d%%", level);
        lv_label_set_text(label_battery, buf);
    }
    if(label_battery_icon) {
        lv_label_set_text(label_battery_icon, battery_symbol_for_level(level));
    }
}

void gui_update_wifi(bool connected) {
    static bool last_connected = false;
    static bool initialized = false;

    if(!label_wifi) {
        return;
    }

    if(initialized && connected == last_connected) {
        return;
    }

    last_connected = connected;
    initialized = true;
    lv_obj_set_style_text_color(label_wifi, connected ? lv_color_hex(0x51D88A) : lv_color_hex(0x59636C), 0);
    lv_obj_set_style_opa(label_wifi, connected ? LV_OPA_COVER : LV_OPA_50, 0);
}
