#!/bin/bash
# Run integration tests for Amnesia Browser
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(dirname "$SCRIPT_DIR")"
CHROMIUM_DIR="$PROJECT_ROOT/chrome/src"
BUILD_DIR="$CHROMIUM_DIR/out/Default"

log_info() {
    echo -e "\033[0;32m[INFO]\033[0m $1"
}

log_error() {
    echo -e "\033[0;31m[ERROR]\033[0m $1"
}

run_integration_tests() {
    log_info "Running integration tests..."
    
    cd "$BUILD_DIR"
    
    if [ -f "amnesia_browser_integration_tests" ]; then
        ./amnesia_browser_integration_tests || {
            log_error "Integration tests failed"
            exit 1
        }
    else
        log_warn "Integration test binary not found. Build may not include tests."
    fi
    
    log_info "Integration tests completed"
}

main() {
    run_integration_tests
}

main "$@"
