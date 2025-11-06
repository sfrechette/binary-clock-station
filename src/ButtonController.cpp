#include "ButtonController.h"

ButtonController::ButtonController()
    : lastBootState(HIGH), lastBrightnessState(HIGH),
      lastBootPress(0), lastBrightnessPress(0),
      brightnessLevel(DEFAULT_BRIGHTNESS_INDEX),
      onTimeToggle(nullptr), onBrightnessChange(nullptr) {
}

void ButtonController::init() {
    pinMode(PIN_BUTTON_BOOT, INPUT_PULLUP);
    pinMode(PIN_BUTTON_IO14, INPUT_PULLUP);
}

void ButtonController::setTimeToggleCallback(void (*callback)()) {
    onTimeToggle = callback;
}

void ButtonController::setBrightnessCallback(void (*callback)(uint8_t)) {
    onBrightnessChange = callback;
}

void ButtonController::update() {
    unsigned long now = millis();
    
    // GPIO 0: Time display toggle
    bool bootState = digitalRead(PIN_BUTTON_BOOT);
    if (bootState == LOW && lastBootState == HIGH) {
        if (now - lastBootPress > BUTTON_DEBOUNCE_MS) {
            if (onTimeToggle) {
                onTimeToggle();
            }
            lastBootPress = now;
        }
    }
    lastBootState = bootState;
    
    // GPIO 14: Brightness cycling
    bool brightnessState = digitalRead(PIN_BUTTON_IO14);
    if (brightnessState == LOW && lastBrightnessState == HIGH) {
        if (now - lastBrightnessPress > BUTTON_DEBOUNCE_MS) {
            brightnessLevel++;
            if (brightnessLevel >= BRIGHTNESS_LEVELS) {
                brightnessLevel = 0;
            }
            
            if (onBrightnessChange) {
                onBrightnessChange(brightnessLevel);
            }
            
            lastBrightnessPress = now;
        }
    }
    lastBrightnessState = brightnessState;
}
