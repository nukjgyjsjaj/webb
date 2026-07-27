#!/bin/bash
# Run consistency validation tests for Amnesia Browser
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

run_consistency_tests() {
    log_info "Running consistency validation tests..."
    
    cd "$BUILD_DIR"
    
    # Run consistency engine tests
    if [ -f "amnesia_consistency_tests" ]; then
        ./amnesia_consistency_tests || {
            log_error "Consistency tests failed"
            exit 1
        }
    else
        log_warn "Consistency test binary not found. Building standalone validator..."
        
        # Build standalone consistency validator if binary not found
        cd "$PROJECT_ROOT/AmnesiaBrowser"
        mkdir -p build_consistency
        cd build_consistency
        cmake .. -DCMAKE_BUILD_TYPE=Release
        make -j$(nproc)
        
        if [ -f "consistency_validator" ]; then
            ./consistency_validator || {
                log_error "Standalone consistency validation failed"
                exit 1
            }
        else
            log_error "Could not build consistency validator"
            exit 1
        fi
    fi
    
    log_info "Consistency validation completed successfully"
}

main() {
    run_consistency_tests
}

main "$@"
