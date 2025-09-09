# RAK11720 Hello World Debug Project

Arduino project for RAK11720 with hardware debugging support via J-Link SWD.

### Required Software
1. **Arduino CLI** - For compiling and uploading sketche
   ```bash
   brew install arduino-cli
   arduino-cli core update-index --additional-urls https://raw.githubusercontent.com/RAKWireless/RAKwireless-Arduino-BSP-Index/main/package_rakwireless_com_rui_index.json
   arduino-cli core install rak_rui:apollo3 --additional-urls https://raw.githubusercontent.com/RAKWireless/RAKwireless-Arduino-BSP-Index/main/package_rakwireless_com_rui_index.json
   ```
2. **Cortex-Debug extension**
- Open VS Code, press Cmd+Shift+X to open Extensions, and search for "Cortex-Debug" by marus25
2. **Cortex-Debug extension**
- Open VS Code, press Cmd+Shift+X to open Extensions, and search for "C/C++" by microsoft
3. **J-Link Software** - For SWD debugging (JLinkExe)
   ```bash
   # macOS (Homebrew)
   brew install segger-jlink
   ```

### VS Code Tasks
- **Verify Sketch**: Ctrl+Shift+P → "Tasks: Run Task" → "Verify Sketch"
- **Upload Sketch**: Ctrl+Shift+P → "Tasks: Run Task" → "Upload Sketch" (upload the code using uart, port needs to be updated in the tasks.json file)
- **Upload via SWD**: Ctrl+Shift+P → "Tasks: Run Task" → "Upload via SWD" (uses J-Link, requires debug build)
- **Start debugging**: Press F5.

