#!/bin/bash
# Fetch Chromium source code using depot_tools
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(dirname "$SCRIPT_DIR")"
CHROMIUM_DIR="$PROJECT_ROOT/chrome"
DEPOT_TOOLS="$PROJECT_ROOT/depot_tools"

export PATH="$DEPOT_TOOLS:$PATH"

log_info() {
    echo -e "\033[0;32m[INFO]\033[0m $1"
}

log_info "Fetching Chromium source code..."

if [ ! -d "$CHROMIUM_DIR" ]; then
    log_info "Running fetch --nohooks chromium..."
    cd "$PROJECT_ROOT"
    fetch --nohooks chromium
else
    log_info "Chromium directory exists, updating..."
    cd "$CHROMIUM_DIR"
    git checkout main
    gclient sync --nohooks
fi

log_info "Chromium source code ready at: $CHROMIUM_DIR"
