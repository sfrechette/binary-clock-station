#ifndef BUTTON_CONTROLLER_H
#define BUTTON_CONTROLLER_H

#include <Arduino.h>
#include "config.h"

class ButtonController {
public:
    ButtonController();
    
    void init();
    void update();
    
    void setTimeToggleCallback(void (*callback)());
    void setBrightnessCallback(void (*callback)(uint8_t));
    
    uint8_t getCurrentBrightnessLevel() const { return brightnessLevel; }
    
private:
    bool lastBootState;
    bool lastBrightnessState;
    unsigned long lastBootPress;
    unsigned long lastBrightnessPress;
    
    uint8_t brightnessLevel;
    
    void (*onTimeToggle)();
    void (*onBrightnessChange)(uint8_t level);
};

#endif // BUTTON_CONTROLLER_H
