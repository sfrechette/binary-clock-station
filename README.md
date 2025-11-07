# Binary Clock Station for T-Diplsay S3

A modern, optimized binary clock implementation for the **LilyGo T-Display-S3** ESP32 board. Displays time in binary format using illuminated dots, with WiFi connectivity for automatic NTP time synchronization.

![ESP32-S3](https://img.shields.io/badge/ESP32-S3-blue)
![Platform](https://img.shields.io/badge/Platform-PlatformIO-orange)
![License](https://img.shields.io/badge/License-MIT-green)

## Features

### Core Functionality

- **Binary Time Display**: Visual binary representation of current time (HH:MM:SS)
- **Optimized LED Layout**: Each column displays only the necessary LEDs for its digit range
  - Hours tens (0-2): 2 LEDs
  - Hours ones (0-9): 4 LEDs
  - Minutes tens (0-5): 3 LEDs
  - Minutes ones (0-9): 4 LEDs
  - Seconds tens (0-5): 3 LEDs
  - Seconds ones (0-9): 4 LEDs
- **NTP Time Sync**: Automatic time synchronization over WiFi
- **Timezone Support**: Configurable timezone (default: EST/EDT)

### User Interface

- **Toggle Time Display** (GPIO 0 / BOOT button): Show/hide decimal time digits below binary display
- **Brightness Control** (GPIO 14 / IO14 button): Cycle through 6 brightness levels (25, 75, 125, 175, 225, 255)
- **Clean Visual Design**:
  - White LEDs for "on" state
  - Light grey LEDs for "off" state
  - Centered display with proper spacing
  - No screen flickering

### Performance

- **Memory Efficient**: Uses only 14% RAM and 11% Flash
- **Fast Updates**: 100ms polling for responsive display
- **Smart Rendering**: Only redraws when time changes
- **Debounced Buttons**: 200ms debounce prevents accidental double-presses

## Prerequisites

### Hardware Requirements

- **LilyGo T-Display-S3** ESP32-S3 development board
  - 1.9" LCD Display (320x170 ST7789)
  - Built-in buttons (GPIO 0 and GPIO 14)
  - WiFi connectivity

### Software Requirements

- **PlatformIO** (VS Code extension or CLI)
- **Git** (for cloning the repository)
- **USB-C cable** (for uploading firmware)

### Network Requirements

- WiFi network with internet access (for NTP time sync)
- WiFi credentials (SSID and password)

## Installation

### 1. Clone the Repository

```bash
git clone https://github.com/yourusername/binary-clock-station.git
cd binary-clock-station
```

### 2. Configure WiFi Credentials

Create a `secrets.h` file in the `include/` directory:

```bash
cp include/secrets_template.h include/secrets.h
```

Edit `include/secrets.h` and add your WiFi credentials:

```cpp
#ifndef SECRETS_H
#define SECRETS_H

// WiFi credentials
#define WIFI_SSID "Your_WiFi_SSID"
#define WIFI_PASS "Your_WiFi_Password"

#endif
```

> **Note**: `secrets.h` is gitignored and will not be committed to version control.

### 3. Configure Timezone (Optional)

Edit `src/config.h` to set your timezone:

```cpp
#define TIMEZONE "EST5EDT,M3.2.0/2,M11.1.0/2"  // Change to your timezone
```

Common timezone strings:

- EST/EDT: `"EST5EDT,M3.2.0/2,M11.1.0/2"`
- PST/PDT: `"PST8PDT,M3.2.0/2,M11.1.0/2"`
- CST/CDT: `"CST6CDT,M3.2.0/2,M11.1.0/2"`
- MST/MDT: `"MST7MDT,M3.2.0/2,M11.1.0/2"`
- UTC: `"UTC0"`

### 4. Build and Upload

#### Using PlatformIO CLI

```bash
# Build the project
pio run

# Upload to device
pio run --target upload

# Monitor serial output
pio device monitor
```

#### Using VS Code with PlatformIO

1. Open the project folder in VS Code
2. Click the PlatformIO icon in the sidebar
3. Select **Build** to compile
4. Select **Upload** to flash the firmware
5. Select **Monitor** to view serial output

### 5. First Boot

On first boot, the device will:

1. Initialize the display
2. Connect to WiFi (may take 10-15 seconds)
3. Synchronize time from NTP servers
4. Display "Binary Clock" splash screen (1 second)
5. Start showing binary time

## Architecture Overview

### Project Structure

```
binary-clock-station/
├── include/
│   ├── secrets.h              # WiFi credentials (gitignored)
│   └── secrets_template.h     # Template for secrets.h
├── src/
│   ├── config.h               # All configuration constants
│   ├── BinaryClockDisplay.h   # Display class header
│   ├── BinaryClockDisplay.cpp # Display rendering logic
│   ├── ButtonController.h     # Button handling class header
│   ├── ButtonController.cpp   # Button debouncing & callbacks
│   └── main.cpp               # Main program orchestration
├── lib/                       # Custom libraries (none currently)
├── test/                      # Unit tests (none currently)
├── platformio.ini             # PlatformIO configuration
├── README.md                  # This file
├── STATUS.md                  # Current project status
├── REFACTOR_SUMMARY.md        # Code refactoring details
└── OPTIMIZATION_APPLIED.md    # Optimization documentation
```

### Module Responsibilities

#### `config.h`

- Centralized configuration constants
- Display dimensions and colors
- Pin assignments
- Timing parameters
- Brightness levels

#### `BinaryClockDisplay` Class

**Responsibility**: Display rendering and visual management

**Key Methods**:

- `init()`: Initialize display hardware and pre-calculate layouts
- `drawClock()`: Render binary clock with optional decimal display
- `setBrightness()`: Adjust backlight brightness
- `drawBCDDigit()`: Draw individual binary digit column
- `drawTimeDigits()`: Draw decimal time digits
- `clearTextArea()`: Clear text display area

**Design Pattern**: Encapsulation of all display-related logic

#### `ButtonController` Class

**Responsibility**: Button input handling with callbacks

**Key Methods**:

- `init()`: Initialize button pins
- `update()`: Poll button states (call in main loop)
- `setTimeToggleCallback()`: Register time display toggle handler
- `setBrightnessCallback()`: Register brightness change handler

**Design Pattern**: Observer pattern with callbacks for loose coupling

#### `main.cpp`

**Responsibility**: System orchestration and business logic

**Key Components**:

- WiFi connection management
- NTP time synchronization
- Application state management
- Event handling coordination
- Main loop execution

## Execution Flow

### Startup Sequence

```
1. Power On / Reset
   ↓
2. Serial Communication Init (115200 baud)
   ↓
3. Display Hardware Init
   ├─ Setup PWM for backlight
   ├─ Initialize TFT display
   ├─ Clear screen (black)
   └─ Pre-calculate digit layouts
   ↓
4. Button Controller Init
   ├─ Setup GPIO pins as inputs
   ├─ Configure pull-ups
   └─ Register callback functions
   ↓
5. WiFi Connection
   ├─ Set WiFi mode to Station
   ├─ Connect to configured network
   └─ Wait for connection (max 15 seconds)
   ↓
6. NTP Time Synchronization
   ├─ Configure timezone
   ├─ Contact NTP servers
   └─ Wait for valid time (max 10 seconds)
   ↓
7. Display Splash Screen
   └─ Show "Binary Clock" for 1 second
   ↓
8. Enter Main Loop
```

### Main Loop Execution

```
Main Loop (executes every 100ms)
│
├─ 1. Update Button States
│  └─ buttonController.update()
│     ├─ Check GPIO 0 (time toggle)
│     │  └─ If pressed: Call onTimeToggle()
│     └─ Check GPIO 14 (brightness)
│        └─ If pressed: Call onBrightnessChange()
│
├─ 2. Get Current Time
│  └─ getLocalTime(&timeinfo)
│     ├─ Success: Continue
│     └─ Failure: Show "NTP?" error, retry
│
├─ 3. Check for Time Change
│  └─ Compare current time with last displayed time
│     ├─ Time changed OR redraw needed?
│     │  ├─ YES:
│     │  │  ├─ Draw binary clock display
│     │  │  ├─ Draw decimal time (if enabled)
│     │  │  └─ Update state variables
│     │  └─ NO: Skip rendering
│     └─ Continue
│
└─ 4. Delay 100ms
   └─ Prevents busy-waiting, reduces power
```

### Button Event Flow

```
GPIO 0 (BOOT Button) Pressed
   ↓
Button Debounce Check (200ms)
   ↓
Valid Press Detected
   ↓
Call onTimeToggle() Callback
   ↓
Toggle appState.showTimeDigits
   ↓
Set appState.needsRedraw = true
   ↓
Next loop iteration redraws display


GPIO 14 Button Pressed
   ↓
Button Debounce Check (200ms)
   ↓
Valid Press Detected
   ↓
Call onBrightnessChange(level) Callback
   ↓
Increment brightness level (0-5, wraps)
   ↓
clockDisplay.setBrightness(level)
   ↓
Adjust PWM backlight intensity
```

### Display Rendering Flow

```
drawClock(hour, minute, second, showDigits)
   ↓
Extract Digits
├─ hours_tens = hour / 10
├─ hours_ones = hour % 10
├─ minutes_tens = minute / 10
├─ minutes_ones = minute % 10
├─ seconds_tens = second / 10
└─ seconds_ones = second % 10
   ↓
For each of 6 columns:
   ↓
   drawBCDDigit(value, layout)
   │
   ├─ Calculate LED positions
   ├─ For each LED in column:
   │  ├─ Check bit value (0 or 1)
   │  ├─ Set color (white or light grey)
   │  └─ Draw filled circle
   └─ Next column
   ↓
If showDigits enabled:
   ↓
   drawTimeDigits()
   ├─ Clear text area
   ├─ Draw decimal digits below each column
   └─ Apply bold effect (draw twice)
   ↓
Complete
```

## Usage

### Controls

| Button | Function | Behavior |
|--------|----------|----------|
| **GPIO 0** (BOOT) | Toggle Time Display | Shows/hides decimal time digits below binary display |
| **GPIO 14** (IO14) | Cycle Brightness | Cycles through 6 levels: 25 → 75 → 125 → 175 → 225 → 255 → 25... |

### Reading the Binary Display

The display shows 6 columns representing: `HH : MM : SS`

Each column is a vertical binary number (read from bottom to top):

- Bottom dot = 1
- Second dot = 2
- Third dot = 4
- Top dot = 8

**Example**: Time is 14:37:52

```
Columns:  1   4   :   3   7   :   5   2
         [ ] [0]  :  [ ] [0]  :  [ ] [0] 
         [ ] [4]     [2] [4]     [4] [2]
         [0] [0]     [1] [2]     [1] [0]
         [1] [0]     [0] [1]     [0] [0]
         ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
          1   4       3   7       5   2
Hours: 1 and 4 = 14 (2:00 PM)
Minutes: 3 and 7 = 37
Seconds: 5 and 2 = 52
```

### Default Settings

- **Time Display**: OFF (hidden)
- **Brightness**: Level 1 (25/255)
- **Update Rate**: 10 times per second (100ms)

## Configuration

### Adjustable Parameters in `config.h`

```cpp
// Display appearance
#define CLOCK_DOT_RADIUS 10        // LED dot size
#define CLOCK_COL_WIDTH 30         // Column width
#define CLOCK_GAP_SMALL 8          // Gap between digit pairs
#define CLOCK_GAP_LARGE 20         // Gap between time units

// Colors
#define BG_COLOR   TFT_BLACK       // Background
#define OFF_COLOR  0x7BEF          // "Off" LED color (light grey)
#define ON_COLOR   TFT_WHITE       // "On" LED color

// Timing
#define TIME_UPDATE_INTERVAL_MS 100   // Loop delay
#define BUTTON_DEBOUNCE_MS 200        // Button debounce time

// Brightness levels (0-255)
static const uint8_t BRIGHTNESS_VALUES[6] = {25, 75, 125, 175, 225, 255};

// NTP Servers
#define NTP_SERVER1 "pool.ntp.org"
#define NTP_SERVER2 "time.google.com"
```

## Memory Usage

```
RAM:   14.0% (45,784 bytes / 327,680 bytes)
Flash: 11.3% (739,545 bytes / 6,553,600 bytes)
```

Plenty of room for future enhancements!

## Troubleshooting

### Display doesn't turn on

- Check USB power supply (needs adequate current)
- Verify `PIN_POWER` and `PIN_BACKLIGHT` definitions
- Try increasing default brightness in `config.h`

### WiFi connection fails

- Verify credentials in `secrets.h`
- Check WiFi signal strength
- Ensure 2.4GHz WiFi (ESP32 doesn't support 5GHz)
- Check serial monitor for error messages

### Time shows "NTP?" error

- WiFi must be connected first
- Check firewall allows NTP (port 123 UDP)
- Try different NTP servers in `config.h`
- Verify internet connectivity

### Buttons not responding

- Check debounce time (200ms default)
- Verify pin definitions match your board
- Try longer press duration
- Check serial monitor for button events

### Display flickers

- Shouldn't happen with optimized code
- If it does, check `TIME_UPDATE_INTERVAL_MS` value
- Ensure stable power supply

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

## Acknowledgments

- **LILYGO** - T-Display S3 hardware
- **TFT_eSPI** - Display library
- **ESP32Time** - Real-time clock management

## Support

- **Issues**: [GitHub Issues](https://github.com/sfrechette/binary-clock-station/issues)

---
