#!/usr/bin/env bash
set -euo pipefail
cd "$(dirname "$0")/.."
source $HOME/.cargo/env
echo "=== cargo test all ==="
cargo test --all 2>&1 | tail -30
