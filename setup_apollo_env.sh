#!/bin/bash

# Define the FQBN for the RAK11720 board
FQBN="rak_rui:apollo3:WisCoreRAK11720Board"

# --- Script Starts Here ---

# 1. Get Project Name
read -p "Enter your project name (e.g., my_rak_project): " PROJECT_NAME

# Check if a project name was provided
if [ -z "$PROJECT_NAME" ]; then
    echo "No project name entered. Exiting."
    exit 1
fi

# 2. Create Project Directories
mkdir -p "${PROJECT_NAME}/.vscode"
echo "Created project directory: ${PROJECT_NAME}"

# 3. Create the tasks.json file
cat > "${PROJECT_NAME}/.vscode/tasks.json" << EOL
{
    "version": "2.0.0",
    "tasks": [
        {
            "label": "Verify Sketch",
            "type": "shell",
            "command": "arduino-cli",
            "args": [
                "compile",
                "--fqbn",
                "${FQBN}",
                "\${fileDirname}"
            ],
            "group": {
                "kind": "build",
                "isDefault": true
            },
            "problemMatcher": []
        },
        {
            "label": "Upload Sketch",
            "type": "shell",
            "command": "arduino-cli",
            "args": [
                "upload",
                "-p",
                "/dev/tty.usbmodemXXXX",
                "--fqbn",
                "${FQBN}",
                "\${fileDirname}"
            ],
            "group": "test",
            "problemMatcher": []
        }
    ]
}
EOL

echo "Created .vscode/tasks.json"

# 4. Create a placeholder c_cpp_properties.json file
cat > "${PROJECT_NAME}/.vscode/c_cpp_properties.json" << EOL
{
    "configurations": [
        {
            "name": "Arduino",
            "includePath": [
                "\${workspaceFolder}/**"
            ],
            "defines": [],
            "macFrameworkPath": [
                "/System/Library/Frameworks",
                "/Library/Frameworks"
            ],
            "compilerPath": "/usr/bin/clang",
            "cStandard": "c11",
            "cppStandard": "c++17",
            "intelliSenseMode": "macos-clang-arm64"
        }
    ],
    "version": 4
}
EOL

echo "Created .vscode/c_cpp_properties.json (placeholder)"

# 5. Create a basic sketch file
cat > "${PROJECT_NAME}/${PROJECT_NAME}.ino" << EOL
void setup() {
  // put your setup code here, to run once:

}

void loop() {
  // put your main code here, to run repeatedly:

}
EOL

echo "Created ${PROJECT_NAME}.ino"

# 6. Print final instructions
echo -e "\n---"
echo "✅ Project setup complete!"
echo ""
echo "IMPORTANT NEXT STEP:"
echo "To enable IntelliSense, run the following command and copy its output to '.vscode/c_cpp_properties.json':"
echo ""
echo "arduino-cli board details -b ${FQBN} --format json > ${PROJECT_NAME}/.vscode/c_cpp_properties.json"
echo ""
echo "Then, open the project in VS Code and remember to change the serial port in 'tasks.json'."

