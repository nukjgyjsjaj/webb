#!/bin/bash
# Apply Amnesia-specific patches to Chromium source
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(dirname "$SCRIPT_DIR")"
CHROMIUM_DIR="$PROJECT_ROOT/chrome/src"
PATCHES_DIR="$PROJECT_ROOT/patches"

log_info() {
    echo -e "\033[0;32m[INFO]\033[0m $1"
}

log_error() {
    echo -e "\033[0;31m[ERROR]\033[0m $1"
}

apply_patches() {
    log_info "Applying Amnesia patches to Chromium..."
    
    if [ ! -d "$CHROMIUM_DIR" ]; then
        log_error "Chromium source not found. Run fetch_chromium.sh first."
        exit 1
    fi
    
    cd "$CHROMIUM_DIR"
    
    # Apply patches in order
    if [ -d "$PATCHES_DIR" ]; then
        for patch in "$PATCHES_DIR"/*.patch; do
            if [ -f "$patch" ]; then
                log_info "Applying patch: $(basename "$patch")"
                git am "$patch" || {
                    log_error "Failed to apply patch: $(basename "$patch")"
                    exit 1
                }
            fi
        done
    else
        log_warn "No patches directory found. Creating placeholder patches..."
        # Placeholder for future patches
        mkdir -p "$PATCHES_DIR"
    fi
    
    log_info "All patches applied successfully"
}

main() {
    apply_patches
}

main "$@"
