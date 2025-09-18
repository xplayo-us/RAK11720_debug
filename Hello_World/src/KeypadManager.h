#ifndef KEYPAD_MANAGER_H
#define KEYPAD_MANAGER_H

#include <Arduino.h>
#include <Adafruit_TCA8418.h>

/*
 * TCA8418 Hardware Connections (from Food Probe v21 schematic Sheet 6):
 *
 * TCA8418 Pin Assignments:
 * - ROW0 (pin 8)  → Keypad Row 0
 * - ROW1 (pin 7)  → Keypad Row 1
 * - ROW2 (pin 6)  → Keypad Row 2
 * - ROW3 (pin 5)  → Keypad Row 3
 * - ROW4 (pin 4)  → Unused
 * - ROW5 (pin 3)  → Unused
 * - ROW6 (pin 2)  → Unused
 * - ROW7 (pin 1)  → LED2 (Red LED)
 *
 * - COL0 (pin 9)  → Keypad Col 0
 * - COL1 (pin 10) → Keypad Col 1
 * - COL2 (pin 11) → Keypad Col 2
 * - COL3 (pin 12) → Keypad Col 3
 * - COL4 (pin 13) → Unused (has pullup R42)
 * - COL5 (pin 14) → Unused (has pullup R39)
 * - COL6 (pin 15) → Unused (has pullup R40)
 * - COL7 (pin 16) → Unused (has pullup R41)
 * - COL8 (pin 17) → Unused (has pullup R46)
 * - COL9 (pin 18) → Unused (has pullup R43)
 *
 * I2C Interface:
 * - SCL (pin 23) → I2C Clock
 * - SDA (pin 22) → I2C Data
 * - INT (pin 24) → Interrupt to RAK11720 (INT_MATRIX signal)
 *
 * I2C Address: 0x34
 *
 * Matrix Configuration: 4 rows × 4 columns
 * Physical Keypad Layout (based on typical 4x4 membrane keypad):
 *     COL0  COL1  COL2  COL3
 * ROW0  1     2     3     A
 * ROW1  4     5     6     B
 * ROW2  7     8     9     C
 * ROW3  *     0     #     D
 */

class KeypadManager {
public:
    KeypadManager();
    bool begin();
    bool hasKeyPressed();
    char getLastKey();
    void printKeyEvent();

    // LED2 control (connected to ROW7/pin 1)
    void setLED2(bool state);
    void toggleLED2();
    bool getLED2State();

    // Buffer processing
    bool processAllEvents();
    char mapKeyToChar(uint8_t row, uint8_t col);

private:
    Adafruit_TCA8418 keypad;
    char lastKey;
    bool led2State;

    // 4x4 keypad mapping based on your schematic
    static const char keyMap[4][4];
};

#endif