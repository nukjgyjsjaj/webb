#!/bin/bash
# Configure Amnesia Browser build
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(dirname "$SCRIPT_DIR")"
CHROMIUM_DIR="$PROJECT_ROOT/chrome/src"

export PATH="$PROJECT_ROOT/depot_tools:$PATH"

log_info() {
    echo -e "\033[0;32m[INFO]\033[0m $1"
}

log_error() {
    echo -e "\033[0;31m[ERROR]\033[0m $1"
}

detect_os() {
    if [[ "$OSTYPE" == "linux-gnu"* ]]; then
        echo "linux"
    elif [[ "$OSTYPE" == "darwin"* ]]; then
        echo "mac"
    elif [[ "$OSTYPE" == "msys" || "$OSTYPE" == "cygwin" ]]; then
        echo "windows"
    else
        echo "unknown"
    fi
}

configure_build() {
    OS=$(detect_os)
    log_info "Configuring build for: $OS"
    
    if [ ! -d "$CHROMIUM_DIR" ]; then
        log_error "Chromium source not found. Run fetch_chromium.sh first."
        exit 1
    fi
    
    cd "$CHROMIUM_DIR"
    
    case $OS in
        linux)
            ARGS_FILE="$PROJECT_ROOT/AmnesiaBrowser/build_configs/linux/args.gn"
            ;;
        mac)
            ARGS_FILE="$PROJECT_ROOT/AmnesiaBrowser/build_configs/mac/args.gn"
            ;;
        windows)
            ARGS_FILE="$PROJECT_ROOT/AmnesiaBrowser/build_configs/windows/args.gn"
            ;;
        *)
            log_error "Unsupported OS: $OS"
            exit 1
            ;;
    esac
    
    mkdir -p out/Default
    gn gen out/Default --args="$(cat "$ARGS_FILE")"
    
    log_info "Build configured successfully at: $CHROMIUM_DIR/out/Default"
}

main() {
    configure_build
}

main "$@"
