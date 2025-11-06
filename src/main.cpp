#include <Arduino.h>
#include <WiFi.h>
#include <time.h>
#include <TFT_eSPI.h>
#include "secret.h"

// ---------- Settings ----------
static const char* TZ_STR = "EST5EDT,M3.2.0/2,M11.1.0/2"; // America/Toronto

// Visual style
#define BG_COLOR   TFT_BLACK
#define OFF_COLOR  TFT_DARKGREY
#define ON_COLOR   TFT_WHITE
#define GRID_COLOR TFT_DARKGREY
#define TEXT_COLOR TFT_WHITE

// Layout
// 170x320 panel (portrait). We'll use landscape for more width.
const int SCREEN_W = 320;
const int SCREEN_H = 170;

TFT_eSPI tft;  // uses Setup206

// Backlight: if running on battery only, LILYGO notes GPIO15 must be HIGH to light the TFT.
const int PIN_BL_ENABLE = 15;

// NTP servers
const char* ntpServer1 = "pool.ntp.org";
const char* ntpServer2 = "time.google.com";

static inline void connectWiFiIfNeeded() {
  // Option A) Hard-coded:
   WiFi.mode(WIFI_STA);
   WiFi.begin(WIFI_SSID, WIFI_PASS);

  // Option B) WiFiManager captive portal (comment out if you don't want it):
  //#include <WiFiManager.h>
  //WiFi.mode(WIFI_STA);
  //WiFiManager wm;
  //wm.setClass("invert"); // nicer looking dark portal
  //wm.autoConnect("TDisplayS3-BinaryClock"); // SSID for config portal if needed
}

static void initTime() {
  configTzTime(TZ_STR, ntpServer1, ntpServer2);
  // Wait until time is set
  time_t now = time(nullptr);
  uint32_t start = millis();
  while (now < 8 * 3600 * 2 && millis() - start < 10000) { // up to ~10s
    delay(200);
    now = time(nullptr);
  }
}

struct DigitLayout {
  int x;          // left of digit block
  int w;          // column width
  int dotR;       // radius of “LED” circle
};

// Draw one BCD digit as a vertical column of circles (rows: 8,4,2,1 top->bottom)
void drawBCDDigit(uint8_t value, const DigitLayout& L) {
  // Value is 0-9. Show as BCD (8,4,2,1).
  // We'll render 4 rows evenly within the available height.
  const int rows = 4;
  const int top = 20;          // grid top offset
  const int bottom = SCREEN_H - 35; // Leave room for text at bottom
  const int gridH = bottom - top;
  const int vSpacing = gridH / rows;
  const int cx = L.x + L.w/2;

  // row order = 8,4,2,1
  const int weights[4] = {8,4,2,1};
  for (int i = 0; i < rows; i++) {
    int cy = top + i * vSpacing + vSpacing/2;
    bool on = (value & weights[i]);
    uint16_t color = on ? ON_COLOR : OFF_COLOR;
    tft.fillCircle(cx, cy, L.dotR, color);
  }
}

// Draw HH:MM:SS as six BCD columns similar to BinaryClock.net
void drawBinaryClock(int h, int m, int s, bool forceRedraw = false) {
  if (forceRedraw) {
    tft.fillScreen(BG_COLOR);
  }

  // Spacing for six digits
  // Layout: Ht Ho  Mt Mo  St So
  const int gapSmall = 8;     // between tens & ones within a pair
  const int gapLarge = 20;    // between hour/minute and minute/second groups
  const int dotR = 10;        // circle radius
  const int colW = 30;        // width per digit column

  // Calculate total width and center it
  const int totalWidth = (6 * colW) + (2 * gapSmall) + (2 * gapLarge) + (2 * gapSmall);
  int x = (SCREEN_W - totalWidth) / 2;

  DigitLayout Ht{ x, colW, dotR }; x += colW + gapSmall;
  DigitLayout Ho{ x, colW, dotR }; x += colW + gapLarge;

  DigitLayout Mt{ x, colW, dotR }; x += colW + gapSmall;
  DigitLayout Mo{ x, colW, dotR }; x += colW + gapLarge;

  DigitLayout St{ x, colW, dotR }; x += colW + gapSmall;
  DigitLayout So{ x, colW, dotR };

  const int top = 20, bottom = SCREEN_H - 35; // Leave room for text at bottom

  // Split into tens/ones
  int hT = h / 10, hO = h % 10;
  int mT = m / 10, mO = m % 10;
  int sT = s / 10, sO = s % 10;

  // **BinaryClock.net style**: simply BCD per digit (weights 8,4,2,1)
  drawBCDDigit(hT, Ht);
  drawBCDDigit(hO, Ho);
  drawBCDDigit(mT, Mt);
  drawBCDDigit(mO, Mo);
  drawBCDDigit(sT, St);
  drawBCDDigit(sO, So);

  // Time text at bottom
  char buf[16];
  snprintf(buf, sizeof(buf), "%02d:%02d:%02d", h, m, s);
  
  // Clear just the text area to prevent overlap
  tft.fillRect(0, 145, SCREEN_W, 25, BG_COLOR);
  
  // Draw the text
  tft.setTextDatum(MC_DATUM);
  tft.setTextColor(TFT_WHITE);
  tft.drawString(buf, SCREEN_W/2, 155, 2);
}

void setup() {
  pinMode(PIN_BL_ENABLE, OUTPUT);
  digitalWrite(PIN_BL_ENABLE, HIGH); // ensure backlight is on (esp. on battery) per LILYGO FAQ

  tft.init();
  // Landscape rotation puts 320px wide x 170px high
  tft.setRotation(1);
  tft.fillScreen(BG_COLOR);

  // WiFi & time
  connectWiFiIfNeeded();
  initTime();

  tft.setTextDatum(MC_DATUM);
  tft.setTextColor(TEXT_COLOR, BG_COLOR);
  tft.drawString("Binary Clock", SCREEN_W/2, SCREEN_H/2 - 10, 4);
  delay(700);
  
  // Clear screen before starting clock
  tft.fillScreen(BG_COLOR);
}

// Track previous time to avoid unnecessary redraws
static int lastH = -1, lastM = -1, lastS = -1;

void loop() {
  // get local time
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    // show a tiny "no time" indicator if NTP failed
    tft.setTextDatum(TR_DATUM);
    tft.setTextColor(TFT_RED, BG_COLOR);
    tft.drawString("NTP?", SCREEN_W - 4, 4, 2);
    delay(500);
    return;
  }

  int h = timeinfo.tm_hour;
  int m = timeinfo.tm_min;
  int s = timeinfo.tm_sec;

  // Only redraw if time has changed
  if (h != lastH || m != lastM || s != lastS) {
    drawBinaryClock(h, m, s);
    lastH = h;
    lastM = m;
    lastS = s;
  }
  
  delay(100); // Check time frequently but only redraw when needed
}