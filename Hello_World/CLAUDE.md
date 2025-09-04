# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

This is an Arduino project for the RAK11720 board (WisBlock Core) running on the Apollo3 platform. The project demonstrates hardware debugging and testing capabilities including LED control, I2C device scanning, analog reading, and SPI flash communication.

## Build Commands

### Compile/Verify Sketch
```bash
arduino-cli compile --fqbn rak_rui:apollo3:WisCoreRAK11720Board .
```

### Upload to Board
```bash
arduino-cli upload -p /dev/cu.usbserial-31101 --fqbn rak_rui:apollo3:WisCoreRAK11720Board .
```

Note: Update the serial port (`/dev/cu.usbserial-31101`) to match your actual device.

### VS Code Tasks
- **Verify Sketch**: Ctrl+Shift+P → "Tasks: Run Task" → "Verify Sketch"
- **Upload Sketch**: Ctrl+Shift+P → "Tasks: Run Task" → "Upload Sketch"  
- **Upload via SWD**: Ctrl+Shift+P → "Tasks: Run Task" → "Upload via SWD" (uses J-Link, requires debug build)
- **Build for Debug**: Ctrl+Shift+P → "Tasks: Run Task" → "Build for Debug" (adds debug symbols)

## Hardware Configuration

### Board Specifications
- **FQBN**: `rak_rui:apollo3:WisCoreRAK11720Board`
- **Platform**: Apollo3 architecture
- **Package**: RAK RUI (RAKwireless)

### Pin Definitions
- `PB5` (pin 38): Green LED
- `PA9` (pin 9): I2C2_SCL/IO6
- `PB3` (pin 13): Analog input AN0
- `PIN_A4`: Main analog pin for readings
- `PIN_SPI_CS`: SPI flash chip select

## Code Architecture

### Main Components

1. **Hardware Initialization** (`setup()`):
   - Serial communication at 115200 baud
   - I2C and SPI bus initialization
   - GPIO pin configuration for LEDs
   - SPI flash detection

2. **Main Loop** (`loop()`):
   - I2C device scanning (addresses 1-126)
   - Analog sensor reading
   - LED state toggling
   - SPI flash communication test
   - 5-second cycle delay

3. **SPI Flash Management** (`detectFlashChip()`):
   - JEDEC ID detection (0x9F command)
   - Expected manufacturer: 0x68 (BoyaMicro/Winbond)
   - Memory type: 0x40, Capacity: 0x18 (128Mbit)

### Development Environment Setup

The project includes a setup script (`../setup_apollo_env.sh`) that creates VS Code configuration for new RAK11720 projects with proper IntelliSense support.

## SWD Debugging with J-Link

### Hardware Connections
Connect RAK11720 to AMA3BEVB debugger:
- SWDIO ↔ SWDIO
- SWCLK ↔ SWCLK  
- GND ↔ GND

### Debug Configurations
Two debug configurations are available:
- **Debug RAK11720 (J-Link)**: Launch mode - builds, loads, and starts debugging
- **Attach to RAK11720 (J-Link)**: Attach mode - connects to already running firmware

### Debug Workflow
1. **Build**: Press `Ctrl+Shift+P` → "Tasks: Run Task" → "Build for Debug"
2. **Debug**: Press **F5** to start debugging
3. **Navigate**: If stuck in system code, use Debug Console: `break setup; continue`
4. **Step**: Use **F10** (step over), **F11** (step into), **F5** (continue)

### Quick Troubleshooting
- **"Cannot find bounds"**: Use `break setup; continue` in Debug Console  
- **Stops in library code**: Use `continue` command  
- **Device not found**: Check SWD connections and J-Link installation

## Serial Monitor

Connect at 115200 baud to see:
- I2C device scan results
- Analog readings
- SPI flash detection status
- Hardware initialization messages