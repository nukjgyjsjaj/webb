# Solana Atomic Arbitrage System

## Quick start
```bash
# 1. Install Rust + Solana (once)
curl --proto '=https' --tlsv1.2 -sSf https://sh.rustup.rs | sh -s -- -y
source $HOME/.cargo/env

# 2. Build & test
cd solana-atomic-arb
cargo build --release
cargo test --all

# 3. Run 5 tests
./scripts/run-tests.sh

# 4. Configure
cp configs/mainnet-paper.toml configs/local.toml
# edit local.toml with your keypair + RPC

# 5. Run paper mode (no sending)
cargo run --bin arb-runner -- --config configs/local.toml --paper
```

## Architecture
- `math-layer`: DEX math (Raydium, Orca, Meteora, Pump)
- `data-layer`: Yellowstone gRPC / Shredstream + BankLedger
- `search-layer`: dirty-set 2/3-hop + Bellman-Ford + size ladder
- `execution-layer`: v0 tx + ALT + simulate + Jito bundle
- `config`: TOML-driven devnet/paper/live

## Docs
- `docs/math.md` — equations
- `docs/tests.md` — 5 mandatory tests
- `docs/limitations.md` — honest constraints
