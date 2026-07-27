#!/bin/bash
# Amnesia Browser Local CI Runner
# This script sets up and runs a local CI server for Amnesia Browser development

set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(dirname "$SCRIPT_DIR")"
BUILD_DIR="$PROJECT_ROOT/out/Default"
DEPOT_TOOLS="$PROJECT_ROOT/depot_tools"

# Colors
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

log_info() {
    echo -e "${GREEN}[INFO]${NC} $1"
}

log_warn() {
    echo -e "${YELLOW}[WARN]${NC} $1"
}

log_error() {
    echo -e "${RED}[ERROR]${NC} $1"
}

# Check dependencies
check_dependencies() {
    log_info "Checking dependencies..."
    
    if ! command -v python3 &> /dev/null; then
        log_error "python3 is required but not installed"
        exit 1
    fi
    
    if ! command -v git &> /dev/null; then
        log_error "git is required but not installed"
        exit 1
    fi
    
    if ! command -v ninja &> /dev/null; then
        log_error "ninja is required but not installed. Install depot_tools first."
        exit 1
    fi
    
    log_info "All dependencies satisfied"
}

# Setup depot_tools
setup_depot_tools() {
    log_info "Setting up depot_tools..."
    
    if [ ! -d "$DEPOT_TOOLS" ]; then
        log_info "Cloning depot_tools..."
        git clone https://chromium.googlesource.com/chromium/tools/depot_tools.git "$DEPOT_TOOLS"
    else
        log_info "depot_tools already exists, updating..."
        cd "$DEPOT_TOOLS"
        git pull
    fi
    
    export PATH="$DEPOT_TOOLS:$PATH"
    log_info "depot_tools setup complete"
}

# Configure build
configure_build() {
    log_info "Configuring build..."
    
    mkdir -p "$BUILD_DIR"
    
    if [[ "$OSTYPE" == "linux-gnu"* ]]; then
        ARGS_FILE="$PROJECT_ROOT/build_configs/linux/args.gn"
    elif [[ "$OSTYPE" == "darwin"* ]]; then
        ARGS_FILE="$PROJECT_ROOT/build_configs/mac/args.gn"
    else
        log_error "Unsupported OS: $OSTYPE"
        exit 1
    fi
    
    cd "$PROJECT_ROOT"
    gn gen out/Default --args="$(cat $ARGS_FILE)"
    
    log_info "Build configured successfully"
}

# Build project
build_project() {
    log_info "Building Amnesia Browser..."
    
    cd "$PROJECT_ROOT"
    autoninja -C out/Default amnesia_browser
    
    log_info "Build completed successfully"
}

# Run tests
run_tests() {
    log_info "Running tests..."
    
    cd "$PROJECT_ROOT"
    
    if [ -f "$BUILD_DIR/amnesia_browser_unittests" ]; then
        "$BUILD_DIR/amnesia_browser_unittests" --gtest_filter='*' || {
            log_error "Unit tests failed"
            exit 1
        }
    fi
    
    if [ -f "$BUILD_DIR/amnesia_browser_integration_tests" ]; then
        "$BUILD_DIR/amnesia_browser_integration_tests" || {
            log_error "Integration tests failed"
            exit 1
        }
    fi
    
    log_info "All tests passed"
}

# Main CI loop
run_ci() {
    log_info "Starting Amnesia Browser Local CI Runner"
    
    check_dependencies
    setup_depot_tools
    configure_build
    build_project
    run_tests
    
    log_info "CI pipeline completed successfully"
}

# Parse arguments
case "${1:-}" in
    "setup")
        check_dependencies
        setup_depot_tools
        configure_build
        ;;
    "build")
        build_project
        ;;
    "test")
        run_tests
        ;;
    "run"|"")
        run_ci
        ;;
    *)
        echo "Usage: $0 [setup|build|test|run]"
        exit 1
        ;;
esac
