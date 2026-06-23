#include <Arduino.h>
#include <lvgl.h>
#include "display_hal.h"
#include "gui.h"
#include "pillbox_logic.h"
#include "network_manager.h"

void setup() {
  Serial.begin(115200);

  // Initialize display and LVGL
  display_hal_init(150); // brightness 150

  // Initialize network manager (WiFi)
  network_init();

  // Initialize hall sensor and logic
  pillbox_logic_init();
}

void loop() {
  static unsigned long lastTimeUpdateAt = 0;

  // Process network portal if in AP mode
  network_update();
  gui_update_wifi(network_is_connected());

  if (millis() - lastTimeUpdateAt >= 1000) {
    char timeText[16];
    if (network_get_local_time(timeText, sizeof(timeText))) {
      gui_update_time(timeText);
    }
    lastTimeUpdateAt = millis();
  }

  // Process pills, sensor debounce and buzzer
  pillbox_logic_update();

  // Process UI animation and touch inputs
  display_hal_update();
  lv_tick_inc(20);

  delay(20);
}

