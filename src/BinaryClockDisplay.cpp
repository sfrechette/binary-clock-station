#include "BinaryClockDisplay.h"
#include "font18.h"

BinaryClockDisplay::BinaryClockDisplay(TFT_eSPI& display) 
    : tft(display), layoutInitialized(false), digitsInitialized(false) {
    // Initialize last displayed digits to invalid values
    for (uint8_t i = 0; i < 6; i++) {
        lastDisplayedDigits[i] = 255;  // Invalid value to force initial draw
    }
}

void BinaryClockDisplay::init() {
    // Setup PWM for backlight
    ledcSetup(PWM_CHANNEL, PWM_FREQ, PWM_RESOLUTION);
    ledcAttachPin(PIN_BACKLIGHT, PWM_CHANNEL);
    ledcWrite(PWM_CHANNEL, BRIGHTNESS_VALUES[DEFAULT_BRIGHTNESS_INDEX]);
    
    // Initialize display power
    pinMode(PIN_POWER, OUTPUT);
    digitalWrite(PIN_POWER, HIGH);
    
    tft.init();
    tft.setRotation(1);
    tft.fillScreen(BG_COLOR);
    
    // Load custom font for time digits
    tft.loadFont(font18);
    
    // Pre-calculate digit layouts with number of bits needed per column
    // Column 0: Hours tens (0-2) = 2 bits
    // Column 1: Hours ones (0-9) = 4 bits
    // Column 2: Minutes tens (0-5) = 3 bits
    // Column 3: Minutes ones (0-9) = 4 bits
    // Column 4: Seconds tens (0-5) = 3 bits
    // Column 5: Seconds ones (0-9) = 4 bits
    const int totalWidth = (6 * CLOCK_COL_WIDTH) + (2 * CLOCK_GAP_SMALL) + (2 * CLOCK_GAP_LARGE) + (2 * CLOCK_GAP_SMALL);
    int16_t x = (int16_t)((SCREEN_W - totalWidth) / 2);
    
    digitLayouts[0].x = x; digitLayouts[0].w = CLOCK_COL_WIDTH; digitLayouts[0].dotR = CLOCK_DOT_RADIUS; digitLayouts[0].numBits = 2;
    x += CLOCK_COL_WIDTH + CLOCK_GAP_SMALL;
    
    digitLayouts[1].x = x; digitLayouts[1].w = CLOCK_COL_WIDTH; digitLayouts[1].dotR = CLOCK_DOT_RADIUS; digitLayouts[1].numBits = 4;
    x += CLOCK_COL_WIDTH + CLOCK_GAP_LARGE;
    
    digitLayouts[2].x = x; digitLayouts[2].w = CLOCK_COL_WIDTH; digitLayouts[2].dotR = CLOCK_DOT_RADIUS; digitLayouts[2].numBits = 3;
    x += CLOCK_COL_WIDTH + CLOCK_GAP_SMALL;
    
    digitLayouts[3].x = x; digitLayouts[3].w = CLOCK_COL_WIDTH; digitLayouts[3].dotR = CLOCK_DOT_RADIUS; digitLayouts[3].numBits = 4;
    x += CLOCK_COL_WIDTH + CLOCK_GAP_LARGE;
    
    digitLayouts[4].x = x; digitLayouts[4].w = CLOCK_COL_WIDTH; digitLayouts[4].dotR = CLOCK_DOT_RADIUS; digitLayouts[4].numBits = 3;
    x += CLOCK_COL_WIDTH + CLOCK_GAP_SMALL;
    
    digitLayouts[5].x = x; digitLayouts[5].w = CLOCK_COL_WIDTH; digitLayouts[5].dotR = CLOCK_DOT_RADIUS; digitLayouts[5].numBits = 4;
    
    layoutInitialized = true;
}

void BinaryClockDisplay::setBrightness(uint8_t level) {
    if (level >= BRIGHTNESS_LEVELS) {
        level = BRIGHTNESS_LEVELS - 1;
    }
    
    // Detach, reattach, write
    ledcDetachPin(PIN_BACKLIGHT);
    delay(10);
    ledcAttachPin(PIN_BACKLIGHT, PWM_CHANNEL);
    ledcWrite(PWM_CHANNEL, BRIGHTNESS_VALUES[level]);
}

void BinaryClockDisplay::drawBCDDigit(uint8_t value, const DigitLayout& layout) {
    const int vSpacing = (CLOCK_BOTTOM - CLOCK_TOP) / 4;
    const int cx = layout.x + layout.w / 2;
    
    static const uint8_t weights[4] = {8, 4, 2, 1};
    
    // Only draw the number of LEDs needed for this column
    for (uint8_t i = 0; i < layout.numBits; i++) {
        // Start from the bottom (least significant bit position)
        uint8_t bitPos = 4 - layout.numBits + i;
        int cy = CLOCK_TOP + bitPos * vSpacing + vSpacing / 2;
        bool on = (value & weights[bitPos]);
        uint16_t color = on ? ON_COLOR : OFF_COLOR;
        tft.fillCircle(cx, cy, layout.dotR, color);
    }
}

void BinaryClockDisplay::clearTextArea() {
    tft.fillRect(0, TEXT_AREA_TOP, SCREEN_W, TEXT_AREA_HEIGHT, BG_COLOR);
}

void BinaryClockDisplay::drawTimeDigits(uint8_t hour, uint8_t minute, uint8_t second,
                                       const DigitLayout layouts[6]) {
    // Extract current digits
    uint8_t digits[6];
    digits[0] = (uint8_t)(hour / 10);
    digits[1] = (uint8_t)(hour % 10);
    digits[2] = (uint8_t)(minute / 10);
    digits[3] = (uint8_t)(minute % 10);
    digits[4] = (uint8_t)(second / 10);
    digits[5] = (uint8_t)(second % 10);
    
    // Setup font once (already loaded in init())
    tft.setTextDatum(MC_DATUM);
    tft.setTextColor(TFT_LIGHTGREY);
    tft.setTextPadding(0);
    
    // First time: clear area and draw all digits
    if (!digitsInitialized) {
        clearTextArea();
        char buf[2] = {0, 0};
        for (uint8_t i = 0; i < 6; i++) {
            buf[0] = '0' + digits[i];
            int cx = layouts[i].x + layouts[i].w / 2;
            tft.drawString(buf, cx, TEXT_Y_POSITION);
            lastDisplayedDigits[i] = digits[i];
        }
        digitsInitialized = true;
    } else {
        // Only update digits that changed (use padding to erase old text smoothly)
        tft.setTextPadding(12);  // Padding width to cover old digit
        char buf[2] = {0, 0};
        for (uint8_t i = 0; i < 6; i++) {
            if (digits[i] != lastDisplayedDigits[i]) {
                int cx = layouts[i].x + layouts[i].w / 2;
                
                // Draw new digit with background padding (automatically erases old)
                buf[0] = '0' + digits[i];
                tft.setTextColor(TFT_LIGHTGREY, BG_COLOR);
                tft.drawString(buf, cx, TEXT_Y_POSITION);
                
                lastDisplayedDigits[i] = digits[i];
            }
        }
        tft.setTextPadding(0);  // Reset padding
    }
}

void BinaryClockDisplay::drawClock(uint8_t hour, uint8_t minute, uint8_t second, bool showDigits) {
    if (!layoutInitialized) {
        return;
    }
    
    // Draw BCD digits
    uint8_t digits[6];
    digits[0] = (uint8_t)(hour / 10);
    digits[1] = (uint8_t)(hour % 10);
    digits[2] = (uint8_t)(minute / 10);
    digits[3] = (uint8_t)(minute % 10);
    digits[4] = (uint8_t)(second / 10);
    digits[5] = (uint8_t)(second % 10);
    
    for (uint8_t i = 0; i < 6; i++) {
        drawBCDDigit(digits[i], digitLayouts[i]);
    }
    
    // Draw time digits if enabled
    if (showDigits) {
        drawTimeDigits(hour, minute, second, digitLayouts);
    } else {
        // Reset digits tracking when hiding
        if (digitsInitialized) {
            clearTextArea();
            digitsInitialized = false;
            for (uint8_t i = 0; i < 6; i++) {
                lastDisplayedDigits[i] = 255;  // Reset to invalid
            }
        }
    }
}
