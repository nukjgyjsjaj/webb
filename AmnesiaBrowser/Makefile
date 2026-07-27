.PHONY: help setup fetch configure build test clean ci

PROJECT_ROOT := $(shell pwd)
CHROMIUM_DIR := $(PROJECT_ROOT)/chrome
DEPOT_TOOLS := $(PROJECT_ROOT)/depot_tools
BUILD_DIR := $(CHROMIUM_DIR)/src/out/Default

export PATH := $(DEPOT_TOOLS):$(PATH)

help: ## Show this help message
	@echo 'Usage: make [target]'
	@echo ''
	@echo 'Available targets:'
	@awk 'BEGIN {FS = ":.*?## "} /^[a-zA-Z_-]+:.*?## / {printf "  %-15s %s\n", $$1, $$2}' $(MAKEFILE_LIST)

setup: ## Setup development environment
	@echo "Setting up development environment..."
	bash scripts/setup_ci.sh

fetch: ## Fetch Chromium source code
	@echo "Fetching Chromium source..."
	bash scripts/fetch_chromium.sh

configure: ## Configure build
	@echo "Configuring build..."
	bash scripts/configure.sh

build: ## Build Amnesia Browser
	@echo "Building Amnesia Browser..."
	bash scripts/build.sh

test: ## Run all tests
	@echo "Running unit tests..."
	bash scripts/run_unit_tests.sh
	@echo "Running integration tests..."
	bash scripts/run_integration_tests.sh
	@echo "Running consistency tests..."
	bash scripts/run_consistency_tests.sh

ci: ## Run full CI pipeline
	@echo "Running CI pipeline..."
	bash scripts/run_ci.sh run

clean: ## Clean build artifacts
	@echo "Cleaning build artifacts..."
	rm -rf $(CHROMIUM_DIR)/src/out
	rm -rf $(CHROMIUM_DIR)/src/.gclient_entries
	rm -rf $(DEPOT_TOOLS)

apply_patches: ## Apply Amnesia patches
	@echo "Applying patches..."
	bash scripts/apply_amnesia_patches.sh

run: ## Run the browser
	@echo "Running Amnesia Browser..."
	@if [ -f "$(BUILD_DIR)/amnesia_browser" ]; then \
		$(BUILD_DIR)/amnesia_browser; \
	elif [ -d "$(BUILD_DIR)/Amnesia Browser.app" ]; then \
		open "$(BUILD_DIR)/Amnesia Browser.app"; \
	elif [ -f "$(BUILD_DIR)/amnesia_browser.exe" ]; then \
		$(BUILD_DIR)/amnesia_browser.exe; \
	else \
		echo "Browser binary not found. Run 'make build' first."; \
		exit 1; \
	fi

.DEFAULT_GOAL := help
