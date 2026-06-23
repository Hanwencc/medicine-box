#pragma once

#include <Arduino.h>

// --------------------------------------------------------
// Pins for Waveshare ESP32-C6-Touch-LCD-1.47
// --------------------------------------------------------

// --- SD Card (SPI) ---
constexpr gpio_num_t PIN_SD_MISO = GPIO_NUM_3;
constexpr gpio_num_t PIN_SD_MOSI = GPIO_NUM_2;
constexpr gpio_num_t PIN_SD_CLK  = GPIO_NUM_1;
constexpr gpio_num_t PIN_SD_CS   = GPIO_NUM_4;

// --- Touch LCD (SPI) ---
// LCD Display
constexpr gpio_num_t PIN_LCD_CLK = GPIO_NUM_1;
constexpr gpio_num_t PIN_LCD_DIN = GPIO_NUM_2;
constexpr gpio_num_t PIN_LCD_CS  = GPIO_NUM_14;
constexpr gpio_num_t PIN_LCD_DC  = GPIO_NUM_15;
constexpr gpio_num_t PIN_LCD_RST = GPIO_NUM_22;
constexpr gpio_num_t PIN_LCD_BL  = GPIO_NUM_23;

// Touch Panel (I2C)
constexpr gpio_num_t PIN_TP_SDA  = GPIO_NUM_18;
constexpr gpio_num_t PIN_TP_SCL  = GPIO_NUM_19;
constexpr gpio_num_t PIN_TP_RST  = GPIO_NUM_20;
constexpr gpio_num_t PIN_TP_INT  = GPIO_NUM_21;

// --- IMU QMI8658 (I2C) ---
constexpr gpio_num_t PIN_IMU_SDA  = GPIO_NUM_18;
constexpr gpio_num_t PIN_IMU_SCL  = GPIO_NUM_19;
constexpr gpio_num_t PIN_IMU_INT1 = GPIO_NUM_5;
constexpr gpio_num_t PIN_IMU_INT2 = GPIO_NUM_6;

// --- UART ---
constexpr gpio_num_t PIN_UART_TX = GPIO_NUM_16;
constexpr gpio_num_t PIN_UART_RX = GPIO_NUM_17;

// --- Native USB ---
constexpr gpio_num_t PIN_USB_DP = GPIO_NUM_13;
constexpr gpio_num_t PIN_USB_DN = GPIO_NUM_12;

// --- Power Control & Buttons ---
constexpr gpio_num_t PIN_RESET = GPIO_NUM_NC;
constexpr gpio_num_t PIN_BOOT  = GPIO_NUM_9; // Usually BOOT is GPIO9 on C6

// Note: I2C pins are shared between IMU and Touch Panel.
constexpr gpio_num_t PIN_I2C_SDA = GPIO_NUM_18;
constexpr gpio_num_t PIN_I2C_SCL = GPIO_NUM_19;

// Note: SPI pins are shared between LCD and SD Card (MOSI, CLK).
constexpr gpio_num_t PIN_SPI_MOSI = GPIO_NUM_2;
constexpr gpio_num_t PIN_SPI_MISO = GPIO_NUM_3; // LCD has no MISO, but SD does
constexpr gpio_num_t PIN_SPI_SCLK = GPIO_NUM_1;
