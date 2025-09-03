#!/bin/bash
# Apollo3 Blue Development Environment Setup Script for macOS
# Target board: AMA3BEVB (Apollo3 Blue, AMA3B1KK-KBR-B0)
# Debug: SEGGER J-Link, VS Code Cortex-Debug
set -euo pipefail

# ---------------------------
# Colored output helpers
# ---------------------------
RED='\033[0;31m'; GREEN='\033[0;32m'; YELLOW='\033[1;33m'; NC='\033[0m'
print_status() { echo -e "${GREEN}[INFO]${NC} $1"; }
print_error()  { echo -e "${RED}[ERROR]${NC} $1"; }
print_warning(){ echo -e "${YELLOW}[WARNING]${NC} $1"; }

# ---------------------------
# Configurable options
# ---------------------------
SDK_SOURCE="${SDK_SOURCE:-ambiq}"   # Use 'ambiq' (official) or 'sparkfun'
SDK_DIR_DEFAULT="$HOME/apollo3-sdk"
PROJECTS_DIR_DEFAULT="$HOME/apollo3-projects"

# ---------------------------
# Pre-run Checks
# ---------------------------
check_macos() {
  if [[ "$OSTYPE" != darwin* ]]; then
    print_error "This script is designed for macOS only"
    exit 1
  fi
  print_status "Running on macOS - OK"
}

detect_architecture() {
  ARCH=$(uname -m)
  if [[ "$ARCH" == "arm64" ]]; then
    HOMEBREW_PREFIX="/opt/homebrew"
    print_status "Detected Apple Silicon Mac (arm64)"
  else
    HOMEBREW_PREFIX="/usr/local"
    print_status "Detected Intel Mac (x86_64)"
  fi
}

# ---------------------------
# Installation Functions
# ---------------------------
install_xcode_tools() {
  print_status "Checking Xcode Command Line Tools..."
  if ! xcode-select -p &>/dev/null; then
    print_status "Installing Xcode Command Line Tools..."
    xcode-select --install || true
    print_warning "Complete the Xcode tools installation in the popup, then re-run this script"
    exit 0
  else
    print_status "Xcode Command Line Tools already installed"
  fi
}

install_homebrew() {
  print_status "Checking Homebrew..."
  if ! command -v brew &>/dev/null; then
    print_status "Installing Homebrew..."
    /bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"
    eval "$($HOMEBREW_PREFIX/bin/brew shellenv)" || true
  else
    print_status "Homebrew already installed"
    brew update
  fi
}

install_tools() {
  print_status "Installing development tools via Homebrew..."

  # ARM GCC Toolchain
  if ! brew list --cask gcc-arm-embedded &>/dev/null; then
    print_status "Installing ARM GCC toolchain (gcc-arm-embedded)..."
    brew install --cask gcc-arm-embedded
  else
    print_status "ARM GCC toolchain already installed."
  fi

  # SEGGER J-Link Software
  if ! brew list --cask segger-jlink &>/dev/null; then
    print_status "Installing Segger J-Link..."
    brew install --cask segger-jlink
  else
    print_status "Segger J-Link already installed."
  fi

  # Python 3 and required packages
  brew install python git cmake ninja make
  print_status "Installing Python packages (pyserial, pycryptodome)..."
  pip3 install --user pyserial pycryptodome
}

setup_sdk() {
  print_status "Setting up Apollo3 SDK..."
  local SDK_DIR="${SDK_DIR_DEFAULT}"
  mkdir -p "$SDK_DIR"
  local AMB_ROOT="$SDK_DIR/AmbiqSuiteSDK"

  if [[ "$SDK_SOURCE" == "sparkfun" ]]; then
    print_status "Using SparkFun AmbiqSuiteSDK fork..."
    if [ -d "$AMB_ROOT" ]; then
      print_warning "SparkFun SDK already exists. Pulling latest changes..."
      (cd "$AMB_ROOT" && git pull && git submodule update --init --recursive)
    else
      (cd "$SDK_DIR" && git clone --recursive https://github.com/sparkfun/AmbiqSuiteSDK.git)
    fi
    # Apply SparkFun-specific fixes
    if [ -f "$AMB_ROOT/tools/apollo3_scripts/keys_info0.py" ]; then
      mv "$AMB_ROOT/tools/apollo3_scripts/keys_info0.py" "$AMB_ROOT/tools/apollo3_scripts/keys_info.py"
    fi
    chmod +x "$AMB_ROOT/tools/apollo3_scripts/"*.py 2>/dev/null || true
  else
    print_status "Using official AmbiqSuite SDK..."
    if [ ! -d "$AMB_ROOT/mcu" ]; then # Check for a key SDK directory
      mkdir -p "$AMB_ROOT"
      print_error "Official AmbiqSuite SDK not found in '$AMB_ROOT'."
      print_warning "Please download the SDK from Ambiq's support portal and extract it there."
      exit 1
    else
      print_status "Official AmbiqSuite SDK found."
    fi
  fi
  
  if [ ! -d "$AMB_ROOT" ]; then
    print_error "SDK root not found. Aborting."
    exit 1
  fi
}

setup_environment() {
  print_status "Setting up environment variables..."
  local AMB_ROOT="$SDK_DIR_DEFAULT/AmbiqSuiteSDK"
  local SHELL_RC=""
  if [[ "$SHELL" == */zsh ]]; then
    SHELL_RC="$HOME/.zshrc"
  else
    SHELL_RC="$HOME/.bash_profile"
  fi
  
  # Avoid adding duplicate entries
  if grep -q "# Apollo3 Development Environment" "$SHELL_RC"; then
    print_warning "Apollo3 environment variables already exist in $SHELL_RC. Skipping."
  else
    print_status "Adding environment variables to $SHELL_RC..."
    cp "$SHELL_RC" "$SHELL_RC.backup_$(date +%Y%m%d_%H%M%S)" 2>/dev/null || true
    cat >> "$SHELL_RC" <<EOF

# Apollo3 Development Environment
export AMB_ROOT="$AMB_ROOT"
export ARM_GCC_PATH="$HOMEBREW_PREFIX/Caskroom/gcc-arm-embedded/10.3-2021.10/gcc-arm-none-eabi-10.3-2021.10" # Adjust this path based on installed version
export PATH="\$ARM_GCC_PATH/bin:\$AMB_ROOT/tools/apollo3_scripts:\$PATH"
EOF
    print_warning "Please run 'source $SHELL_RC' or restart your terminal to apply changes."
  fi
}

install_vscode_extensions() {
  if command -v code &>/dev/null; then
    print_status "Installing VS Code extensions..."
    code --install-extension ms-vscode.cpptools
    code --install-extension marus25.cortex-debug
    code --install-extension ms-vscode.makefile-tools
  else
    print_warning "VS Code 'code' command not in PATH. Please install extensions manually."
  fi
}

# ---------------------------
# Main Execution
# ---------------------------
main() {
  print_status "Starting Apollo3 Development Environment Setup..."
  
  check_macos
  detect_architecture
  install_xcode_tools
  install_homebrew
  install_tools
  setup_sdk
  setup_environment
  install_vscode_extensions

  print_status "Setup script finished successfully!"
  print_warning "Next Steps:"
  echo "1. Source your shell profile (e.g., 'source ~/.zshrc') or restart your terminal."
  echo "2. You can now create and build projects using the AmbiqSuite SDK examples."
  echo "3. A 'Hello World' project can be created by adapting an example from '$SDK_DIR_DEFAULT/AmbiqSuiteSDK/examples'."
}

main "$@"

