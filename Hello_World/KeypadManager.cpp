#include "KeypadManager.h"

// Define the keypad layout for your 4x4 matrix
const char KeypadManager::keyMap[4][4] = {
    {'1', '2', '3', 'A'},
    {'4', '5', '6', 'B'},
    {'7', '8', '9', 'C'},
    {'*', '0', '#', 'D'}
};

KeypadManager::KeypadManager() : lastKey(0) {
}

bool KeypadManager::begin() {
    if (!keypad.begin()) {
        Serial.println("Failed to initialize TCA8418 keypad!");
        return false;
    }

    Serial.println("TCA8418 keypad initialized successfully!");

    // Configure for 4x4 matrix (4 rows, 4 columns)
    keypad.matrix(4, 4);

    return true;
}

bool KeypadManager::hasKeyPressed() {
    return keypad.available() > 0;
}

char KeypadManager::getLastKey() {
    if (hasKeyPressed()) {
        keypadEvent e = keypad.read();
        if (e.bit.EVENT == KEY_JUST_PRESSED) {
            lastKey = mapKeyToChar(e.bit.ROW, e.bit.COL);
            return lastKey;
        }
    }
    return 0; // No key pressed
}

char KeypadManager::mapKeyToChar(uint8_t row, uint8_t col) {
    if (row < 4 && col < 4) {
        return keyMap[row][col];
    }
    return '?'; // Invalid key
}

void KeypadManager::printKeyEvent() {
    if (hasKeyPressed()) {
        keypadEvent e = keypad.read();

        Serial.print("Key at Row ");
        Serial.print(e.bit.ROW);
        Serial.print(", Col ");
        Serial.print(e.bit.COL);
        Serial.print(" was ");

        if (e.bit.EVENT == KEY_JUST_PRESSED) {
            Serial.print("pressed");
            char key = mapKeyToChar(e.bit.ROW, e.bit.COL);
            Serial.print(" (");
            Serial.print(key);
            Serial.print(")");
        } else if (e.bit.EVENT == KEY_JUST_RELEASED) {
            Serial.print("released");
        }

        Serial.println();
    }
}