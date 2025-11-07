#ifndef BINARY_CLOCK_DISPLAY_H
#define BINARY_CLOCK_DISPLAY_H

#include <TFT_eSPI.h>
#include "config.h"

class BinaryClockDisplay {
public:
    BinaryClockDisplay(TFT_eSPI& display);
    
    void init();
    void drawClock(uint8_t hour, uint8_t minute, uint8_t second, bool showDigits);
    void setBrightness(uint8_t level);
    
private:
    TFT_eSPI& tft;
    
    struct DigitLayout {
        int16_t x;
        uint8_t w;
        uint8_t dotR;
        uint8_t numBits;  // Number of LEDs to display for this column
    };
    
    void drawBCDDigit(uint8_t value, const DigitLayout& layout);
    void drawTimeDigits(uint8_t hour, uint8_t minute, uint8_t second, 
                       const DigitLayout layouts[6]);
    void clearTextArea();
    
    DigitLayout digitLayouts[6];
    bool layoutInitialized;
};

#endif // BINARY_CLOCK_DISPLAY_H
