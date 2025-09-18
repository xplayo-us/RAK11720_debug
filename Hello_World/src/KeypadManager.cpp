#include "KeypadManager.h"

// Define the keypad layout for your 4x4 matrix
const char KeypadManager::keyMap[4][4] = {
    {'1', '2', '3', 'A'},
    {'4', '5', '6', 'B'},
    {'7', '8', '9', 'C'},
    {'*', '0', '#', 'D'}
};

KeypadManager::KeypadManager() : lastKey(0), led2State(false) {
}

bool KeypadManager::begin() {
    if (!keypad.begin()) {
        Serial.println("Failed to initialize TCA8418 keypad!");
        return false;
    }

    Serial.println("TCA8418 keypad initialized successfully!");

    // Configure for 4x4 matrix (4 rows, 4 columns)
    keypad.matrix(4, 4);

    // Configure ROW7 (TCA8418_ROW7) as GPIO output for LED2
    keypad.pinMode(TCA8418_ROW7, OUTPUT);
    setLED2(false); // Start with LED off

    // Configure unused pins as inputs to prevent floating (reduce phantom events)
    // Unused ROW pins: ROW4, ROW5, ROW6
    keypad.pinMode(TCA8418_ROW4, INPUT);
    keypad.pinMode(TCA8418_ROW5, INPUT);
    keypad.pinMode(TCA8418_ROW6, INPUT);

    // Unused COL pins: COL4, COL5, COL6, COL7, COL8, COL9
    keypad.pinMode(TCA8418_COL4, INPUT);
    keypad.pinMode(TCA8418_COL5, INPUT);
    keypad.pinMode(TCA8418_COL6, INPUT);
    keypad.pinMode(TCA8418_COL7, INPUT);
    keypad.pinMode(TCA8418_COL8, INPUT);
    keypad.pinMode(TCA8418_COL9, INPUT);

    // Enable debouncing to reduce noise
    keypad.enableDebounce();

    // Clear any stale events from buffer
    keypad.flush();
    Serial.println("Unused pins configured as inputs, debouncing enabled, buffer cleared");

    return true;
}

bool KeypadManager::hasKeyPressed() {
    return keypad.available() > 0;
}

char KeypadManager::getLastKey() {
    if (hasKeyPressed()) {
        uint8_t event = keypad.getEvent();
        // Check if it's a key press (bit 7 = 1 means press, bit 7 = 0 means release)
        if (event & 0x80) {
            // TCA8418 encoding from datasheet: subtract 1, then row=k/10, col=k%10
            uint8_t k = (event & 0x7F) - 1;
            uint8_t row = k / 10;
            uint8_t col = k % 10;
            lastKey = mapKeyToChar(row, col);
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
        uint8_t event = keypad.getEvent();

        // Extract row and column from event using correct TCA8418 encoding
        uint8_t k = (event & 0x7F) - 1;
        uint8_t row = k / 10;
        uint8_t col = k % 10;
        bool isPress = (event & 0x80) != 0;

        Serial.print("Raw event: 0x");
        Serial.print(event, HEX);
        Serial.print(" -> Row ");
        Serial.print(row);
        Serial.print(", Col ");
        Serial.print(col);
        Serial.print(" was ");
        Serial.print(isPress ? "pressed" : "released");

        if (isPress) {
            Serial.print(" (");
            Serial.print(mapKeyToChar(row, col));
            Serial.print(")");
        }

        Serial.println();
    }
}

// LED2 Control Functions
void KeypadManager::setLED2(bool state) {
    led2State = state;
    keypad.digitalWrite(TCA8418_ROW7, state ? HIGH : LOW);
}

void KeypadManager::toggleLED2() {
    setLED2(!led2State);
}

bool KeypadManager::getLED2State() {
    return led2State;
}

// Process all events in buffer and return true if any key press was detected
bool KeypadManager::processAllEvents() {
    if (!hasKeyPressed()) {
        return false;
    }

    int eventCount = keypad.available();
    Serial.print("Processing ");
    Serial.print(eventCount);
    Serial.println(" events:");

    bool keyPressDetected = false;

    while (hasKeyPressed()) {
        uint8_t event = keypad.getEvent();

        // Parse the event using existing TCA8418 encoding
        uint8_t k = (event & 0x7F) - 1;
        uint8_t row = k / 10;
        uint8_t col = k % 10;
        bool isPress = (event & 0x80) != 0;

        // Validate event: TCA8418 only has ROW0-7, COL0-9, but we only use ROW0-3, COL0-3
        if (row > 7 || col > 9) {
            Serial.print("  INVALID EVENT: Raw=0x");
            Serial.print(event, HEX);
            Serial.print(" -> Row ");
            Serial.print(row);
            Serial.print(", Col ");
            Serial.print(col);
            Serial.println(" (impossible position - ignoring)");
            continue; // Skip this invalid event
        }

        // Additional filter: only process events from our 4x4 matrix
        if (row >= 4 || col >= 4) {
            Serial.print("  IGNORED: Row ");
            Serial.print(row);
            Serial.print(", Col ");
            Serial.print(col);
            Serial.println(" (outside 4x4 matrix)");
            continue; // Skip events from unused pins
        }

        // Use existing mapKeyToChar function
        char keyChar = mapKeyToChar(row, col);

        Serial.print("  Row ");
        Serial.print(row);
        Serial.print(", Col ");
        Serial.print(col);
        Serial.print(" - ");
        Serial.print(isPress ? "PRESSED" : "RELEASED");
        Serial.print(" (");
        Serial.print(keyChar);
        Serial.println(")");

        // Track if any key press was detected (not just release)
        if (isPress) {
            keyPressDetected = true;
        }
    }

    return keyPressDetected;
}