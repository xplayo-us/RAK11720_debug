#include <Wire.h>
#include <SPI.h>
#include "src/KeypadManager.h"

#define PB5                 38 // LED GREEN
#define PA9                 9  // IO6 or I2C2_SCL
#define PB3                 13 // AN0
#define analogPin           PIN_A4  // Or use PIN_A0
#define FLASH_CS_PIN        PIN_SPI_CS  // <<< Define your CS pin for SPI flash here

uint8_t ledPin1 = PB5;
uint8_t ledPin2 = P4;
uint8_t ledPin3 = PIN_A0;

int val = 0, val2;
bool state = false;
bool ledSwitch = false;

SPISettings spiFlash(1000000, MSBFIRST, SPI_MODE0);

// Initialize keypad manager
KeypadManager keypad;

void setup()
{
    Wire.begin();
    Serial.begin(115200);
    delay(2000);

    Serial.println("RAKwireless Arduino LED Breathing Example");
    Serial.println("------------------------------------------------------");

    pinMode(ledPin1, OUTPUT);
    pinMode(ledPin2, OUTPUT);

    // SPI flash setup
    pinMode(FLASH_CS_PIN, OUTPUT);
    digitalWrite(FLASH_CS_PIN, HIGH);  // Deselect flash
    SPI.begin();

    // Run JEDEC ID check
    detectFlashChip();

    // Initialize keypad
    if (!keypad.begin()) {
        Serial.println("WARNING: Keypad initialization failed!");
    }
}

void detectFlashChip()
{
    uint8_t manufacturer, memType, capacity;

    SPI.beginTransaction(spiFlash);
    digitalWrite(FLASH_CS_PIN, LOW);
    SPI.transfer(0x9F); // JEDEC ID command
    manufacturer = SPI.transfer(0x00);
    memType = SPI.transfer(0x00);
    capacity = SPI.transfer(0x00);
    digitalWrite(FLASH_CS_PIN, HIGH);
    SPI.endTransaction();

    Serial.println("SPI Flash JEDEC ID:");
    Serial.print("Manufacturer ID: 0x"); Serial.println(manufacturer, HEX);
    Serial.print("Memory Type:    0x"); Serial.println(memType, HEX);
    Serial.print("Capacity ID:    0x"); Serial.println(capacity, HEX);

    // Match expected values (common Winbond/BoyaMicro 128Mbit)
    if (manufacturer == 0x68 && memType == 0x40 && capacity == 0x18) {
        Serial.println("Flash chip detected!");
    } else {
        Serial.println("Flash chip not detected or ID mismatch.");
    }
}

void loop()
{
    byte error, address;
    int nDevices;
    digitalWrite(ledPin2, LOW); // active NFC regulator
    val2 = analogRead(analogPin);
    Serial.println(val2);
    Serial.println("Scanning I2C DEVICES..."); 
        nDevices = 0;
    for (address = 1; address < 127; address++) {
        Wire.beginTransmission(address);
        error = Wire.endTransmission();

        if (error == 0) {
            Serial.print("I2C device found at address 0x");
            if (address < 16) Serial.print("0");
            Serial.print(address, HEX);
            Serial.println("  !");
            nDevices++;
        } else if (error == 4) {
            Serial.print("Unknown error at address 0x");
            if (address < 16) Serial.print("0");
            Serial.println(address, HEX);
        }
    }

    if (nDevices == 0)
        Serial.println("No I2C devices found\n");
    else
        Serial.println("done\n");

    if (val == LOW){
        digitalWrite(ledPin1, HIGH);
        val = HIGH;
    }
    else {
        digitalWrite(ledPin1, LOW);
        val = LOW;
    }
    //CHeck SPI FLASH communication
    detectFlashChip();

    // Check for keypad input
    if (keypad.hasKeyPressed()) {
        char key = keypad.getLastKey();
        if (key != 0) {
            Serial.print("Key pressed: ");
            Serial.println(key);

            // Toggle LED2 when any key is pressed
            keypad.toggleLED2();
            Serial.print("LED2 is now: ");
            Serial.println(keypad.getLED2State() ? "ON" : "OFF");
        }
        keypad.printKeyEvent(); // Detailed event information
    }

    delay(5000);
}