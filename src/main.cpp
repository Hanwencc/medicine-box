#include <Arduino.h>
#include <lvgl.h>
#include "display_hal.h"
#include "pillbox_logic.h"

void setup() {
  Serial.begin(115200);

  // Initialize display and LVGL
  display_hal_init(150); // brightness 150

  // Initialize hall sensor and logic
  pillbox_logic_init();
}

void loop() {
  // Process pills, sensor debounce and buzzer
  pillbox_logic_update();

  // Process UI animation and touch inputs
  display_hal_update();
  lv_tick_inc(20);

  delay(20);
}

