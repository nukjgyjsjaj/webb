#!/bin/bash
# Run unit tests for Amnesia Browser
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

run_unit_tests() {
    log_info "Running unit tests..."
    
    cd "$BUILD_DIR"
    
    # Find and run all unit test binaries
    TEST_BINARIES=(
        "amnesia_session_unittests"
        "amnesia_fingerprint_unittests"
        "amnesia_tls_unittests"
        "amnesia_vless_unittests"
        "amnesia_consistency_unittests"
        "amnesia_cleanup_unittests"
        "amnesia_storage_unittests"
    )
    
    FAILED_TESTS=0
    
    for test in "${TEST_BINARIES[@]}"; do
        if [ -f "$test" ]; then
            log_info "Running: $test"
            ./"$test" --gtest_filter='*' || {
                log_error "Test failed: $test"
                FAILED_TESTS=$((FAILED_TESTS + 1))
            }
        else
            log_warn "Test binary not found: $test"
        fi
    done
    
    if [ $FAILED_TESTS -eq 0 ]; then
        log_info "All unit tests passed!"
    else
        log_error "$FAILED_TESTS test suite(s) failed"
        exit 1
    fi
}

main() {
    run_unit_tests
}

main "$@"
