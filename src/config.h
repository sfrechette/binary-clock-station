#ifndef CONFIG_H
#define CONFIG_H

#include <TFT_eSPI.h>

// ==================== DISPLAY CONFIGURATION ====================
#define SCREEN_W 320
#define SCREEN_H 170
#define BG_COLOR   TFT_BLACK
#define OFF_COLOR  0x7BEF  // Light grey
#define ON_COLOR   TFT_WHITE
#define TEXT_COLOR TFT_WHITE

// ==================== PIN CONFIGURATION ====================
#define PIN_POWER 15
#define PIN_BACKLIGHT 38
#define PIN_BUTTON_BOOT 0
#define PIN_BUTTON_IO14 14

// ==================== BRIGHTNESS CONFIGURATION ====================
#define PWM_CHANNEL 0
#define PWM_FREQ 10000
#define PWM_RESOLUTION 8
#define BRIGHTNESS_LEVELS 6
#define DEFAULT_BRIGHTNESS_INDEX 0

static const uint8_t BRIGHTNESS_VALUES[BRIGHTNESS_LEVELS] = {25, 75, 125, 175, 225, 255};

// ==================== BUTTON CONFIGURATION ====================
#define BUTTON_DEBOUNCE_MS 200

// ==================== TIME CONFIGURATION ====================
#define TIME_UPDATE_INTERVAL_MS 100
#define NTP_SERVER1 "pool.ntp.org"
#define NTP_SERVER2 "time.google.com"
#define TIMEZONE "EST5EDT,M3.2.0/2,M11.1.0/2"

// ==================== CLOCK DISPLAY CONFIGURATION ====================
#define CLOCK_TOP 20
#define CLOCK_BOTTOM 135
#define CLOCK_GAP_SMALL 8
#define CLOCK_GAP_LARGE 20
#define CLOCK_DOT_RADIUS 10
#define CLOCK_COL_WIDTH 30

#define TEXT_AREA_TOP 145
#define TEXT_AREA_HEIGHT 25
#define TEXT_Y_POSITION 155

#endif // CONFIG_H
