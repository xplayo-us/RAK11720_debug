#ifndef KEYPAD_MANAGER_H
#define KEYPAD_MANAGER_H

#include <Arduino.h>
#include <Adafruit_TCA8418.h>

class KeypadManager {
public:
    KeypadManager();
    bool begin();
    bool hasKeyPressed();
    char getLastKey();
    void printKeyEvent();

private:
    Adafruit_TCA8418 keypad;
    char lastKey;

    // 4x4 keypad mapping based on your schematic
    static const char keyMap[4][4];
    char mapKeyToChar(uint8_t row, uint8_t col);
};

#endif