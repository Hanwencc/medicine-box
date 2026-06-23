#include "pillbox_logic.h"
#include <Arduino.h>
#include <esp_sleep.h>
#include "board_pins.h"
#include "gui.h"

// Configuration
static constexpr gpio_num_t PIN_HALL = GPIO_NUM_7;
static constexpr uint8_t PIN_BUZZER = 10;
static constexpr bool HALL_ACTIVE_LOW = true;
static constexpr uint32_t HALL_DEBOUNCE_MS = 80;
static constexpr uint32_t REMINDER_BEEP_INTERVAL_MS = 1000;
static constexpr uint32_t REMINDER_WINDOW_MINUTES = 60;

enum class AppState {
  Idle,
  Reminding,
  LidOpened,
  Taken,
  Missed
};

static AppState state = AppState::Idle;
static bool lastStableLidOpen = false;
static bool lastRawLidOpen = false;
static uint32_t lastHallChangeMs = 0;
static bool hadOpenInCurrentWindow = false;
static uint32_t reminderStartedMs = 0;
static uint32_t lastBeepMs = 0;

static bool readHallRawOpen() {
  const bool active = digitalRead(PIN_HALL) == (HALL_ACTIVE_LOW ? LOW : HIGH);
  const bool lidClosed = active;
  return !lidClosed; // If not closed, it's open
}

static bool readHallDebouncedOpen() {
  const bool raw = readHallRawOpen();
  const uint32_t now = millis();

  if (raw != lastRawLidOpen) {
    lastRawLidOpen = raw;
    lastHallChangeMs = now;
  }

  if ((now - lastHallChangeMs) >= HALL_DEBOUNCE_MS && raw != lastStableLidOpen) {
    lastStableLidOpen = raw;
  }

  return lastStableLidOpen;
}

static void beepOnce(uint16_t freq = 2400, uint16_t ms = 120) {
  tone(PIN_BUZZER, freq, ms);
}

static void startReminder() {
  state = AppState::Reminding;
  reminderStartedMs = millis();
  hadOpenInCurrentWindow = false;
  beepOnce();
}

static bool isReminderWindowExpired() {
  const uint32_t elapsedMs = millis() - reminderStartedMs;
  return elapsedMs > REMINDER_WINDOW_MINUTES * 60UL * 1000UL;
}

static void enterLowPowerSleep() {
  constexpr uint64_t fallbackWakeUs = 10ULL * 60ULL * 1000000ULL;
  esp_sleep_enable_timer_wakeup(fallbackWakeUs);

  const uint64_t hallMask = 1ULL << static_cast<uint8_t>(PIN_HALL);
  const esp_sleep_ext1_wakeup_mode_t wakeMode =
      HALL_ACTIVE_LOW ? ESP_EXT1_WAKEUP_ANY_HIGH : ESP_EXT1_WAKEUP_ANY_LOW;
  esp_sleep_enable_ext1_wakeup_io(hallMask, wakeMode);

  esp_deep_sleep_start();
}

static void markTaken() {
  state = AppState::Taken;
  noTone(PIN_BUZZER);
  // Persist log to NVS and update UI
}

static void markMissed() {
  state = AppState::Missed;
  noTone(PIN_BUZZER);
  // Persist log
}

static void handleHallEvent(bool lidOpen) {
  static bool previousOpen = false;

  if (lidOpen != previousOpen) {
    if (lidOpen) {
      hadOpenInCurrentWindow = true;
      state = AppState::LidOpened;
    } else {
      if (hadOpenInCurrentWindow && state == AppState::LidOpened) {
        markTaken();
      }
    }
    previousOpen = lidOpen;
  }
}

static void updateReminder() {
  if (state != AppState::Reminding && state != AppState::LidOpened) {
    return;
  }

  const uint32_t now = millis();
  if (now - lastBeepMs >= REMINDER_BEEP_INTERVAL_MS) {
    lastBeepMs = now;
    beepOnce();
  }

  if (isReminderWindowExpired()) {
    markMissed();
  }
}

void pillbox_logic_init(void) {
  pinMode(PIN_HALL, INPUT_PULLUP);
  pinMode(PIN_BUZZER, OUTPUT);
  noTone(PIN_BUZZER);

  lastStableLidOpen = readHallRawOpen();
  lastRawLidOpen = lastStableLidOpen;

  // Start reminder for prototype testing
  startReminder();
}

void pillbox_logic_update(void) {
  const bool lidOpen = readHallDebouncedOpen();
  handleHallEvent(lidOpen);
  updateReminder();

  if (state == AppState::Taken || state == AppState::Missed) {
    delay(1000);
    enterLowPowerSleep();
  }
}
