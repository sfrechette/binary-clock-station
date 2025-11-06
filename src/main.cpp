#include <Arduino.h>
#include <WiFi.h>
#include <time.h>
#include <TFT_eSPI.h>
#include "secrets.h"
#include "config.h"
#include "BinaryClockDisplay.h"
#include "ButtonController.h"

// ==================== GLOBAL OBJECTS ====================
TFT_eSPI tft;
BinaryClockDisplay clockDisplay(tft);
ButtonController buttonController;

// ==================== STATE VARIABLES ====================
static struct {
    int8_t lastHour = -1;
    int8_t lastMinute = -1;
    int8_t lastSecond = -1;
    bool showTimeDigits = false;
    bool needsRedraw = true;
} appState;

// ==================== WIFI & TIME FUNCTIONS ====================
static inline void connectWiFi() {
    WiFi.mode(WIFI_STA);
    WiFi.begin(WIFI_SSID, WIFI_PASS);
    
    Serial.print("Connecting to WiFi");
    uint8_t attempts = 0;
    while (WiFi.status() != WL_CONNECTED && attempts < 30) {
        delay(500);
        Serial.print(".");
        attempts++;
    }
    
    if (WiFi.status() == WL_CONNECTED) {
        Serial.println("\nWiFi connected");
        Serial.print("IP: ");
        Serial.println(WiFi.localIP());
    } else {
        Serial.println("\nWiFi connection failed!");
    }
}

static void initTime() {
    configTzTime(TIMEZONE, NTP_SERVER1, NTP_SERVER2);
    
    time_t now = time(nullptr);
    uint32_t start = millis();
    while (now < 8 * 3600 * 2 && millis() - start < 10000) {
        delay(200);
        now = time(nullptr);
    }
    
    Serial.println("Time synchronized");
}

// ==================== CALLBACK FUNCTIONS ====================
void onTimeToggle() {
    appState.showTimeDigits = !appState.showTimeDigits;
    appState.needsRedraw = true;
    Serial.printf("Time display: %s\n", appState.showTimeDigits ? "ON" : "OFF");
}

void onBrightnessChange(uint8_t level) {
    clockDisplay.setBrightness(level);
    Serial.printf("Brightness: level %d/%d (%d/255)\n", 
                  level + 1, BRIGHTNESS_LEVELS, BRIGHTNESS_VALUES[level]);
}

// ==================== SETUP ====================
void setup() {
    Serial.begin(115200);
    Serial.println("\n\n=== Binary Clock (Optimized) ===");
    
    // Initialize display
    clockDisplay.init();
    Serial.println("Display initialized");
    
    // Initialize buttons
    buttonController.init();
    buttonController.setTimeToggleCallback(onTimeToggle);
    buttonController.setBrightnessCallback(onBrightnessChange);
    Serial.println("Buttons initialized");
    
    // Connect WiFi and sync time
    connectWiFi();
    initTime();
    
    // Show startup message
    tft.setTextDatum(MC_DATUM);
    tft.setTextColor(TEXT_COLOR, BG_COLOR);
    tft.drawString("Binary Clock", SCREEN_W/2, SCREEN_H/2 - 10, 4);
    delay(1000);
    tft.fillScreen(BG_COLOR);
    
    Serial.println("=== Binary Clock Ready ===");
    Serial.println("GPIO 0: Toggle time display");
    Serial.println("GPIO 14: Cycle brightness");
}

// ==================== MAIN LOOP ====================
void loop() {
    // Update button states
    buttonController.update();
    
    // Get current time
    struct tm timeinfo;
    if (!getLocalTime(&timeinfo)) {
        tft.setTextDatum(TR_DATUM);
        tft.setTextColor(TFT_RED, BG_COLOR);
        tft.drawString("NTP?", SCREEN_W - 4, 4, 2);
        delay(500);
        return;
    }
    
    // Check if time has changed
    int8_t h = (int8_t)timeinfo.tm_hour;
    int8_t m = (int8_t)timeinfo.tm_min;
    int8_t s = (int8_t)timeinfo.tm_sec;
    
    if (h != appState.lastHour || m != appState.lastMinute || s != appState.lastSecond || appState.needsRedraw) {
        // Update display
        clockDisplay.drawClock((uint8_t)h, (uint8_t)m, (uint8_t)s, appState.showTimeDigits);
        
        // Update state
        appState.lastHour = h;
        appState.lastMinute = m;
        appState.lastSecond = s;
        appState.needsRedraw = false;
    }
    
    // Small delay
    delay(TIME_UPDATE_INTERVAL_MS);
}
