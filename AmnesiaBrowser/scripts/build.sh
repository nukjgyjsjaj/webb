#!/bin/bash
# Build Amnesia Browser
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

build_amnesia() {
    log_info "Building Amnesia Browser..."
    
    if [ ! -d "$CHROMIUM_DIR" ]; then
        log_error "Chromium source not found. Run fetch_chromium.sh first."
        exit 1
    fi
    
    if [ ! -d "$CHROMIUM_DIR/out/Default" ]; then
        log_error "Build not configured. Run configure.sh first."
        exit 1
    fi
    
    cd "$CHROMIUM_DIR"
    
    log_info "Starting build..."
    autoninja -C out/Default amnesia_browser
    
    if [ -f "out/Default/amnesia_browser" ] || [ -f "out/Default/Amnesia Browser.app" ] || [ -f "out/Default/amnesia_browser.exe" ]; then
        log_info "Build successful!"
        log_info "Binary located at: $CHROMIUM_DIR/out/Default/"
    else
        log_error "Build failed - binary not found"
        exit 1
    fi
}

main() {
    build_amnesia
}

main "$@"
