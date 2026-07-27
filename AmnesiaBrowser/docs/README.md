# Amnesia Browser - Initial Documentation

## Building from Source

### Prerequisites
- depot_tools (https://chromium.googlesource.com/chromium/tools/depot_tools.git)
- GN/Ninja
- LLVM/Clang
- Python 3.9+
- CMake 3.20+
- Go 1.21+ (for uTLS)
- Git

### Linux Build
```bash
# Setup depot_tools
git clone https://chromium.googlesource.com/chromium/tools/depot_tools.git ~/depot_tools
export PATH=~/depot_tools:$PATH

# Fetch Chromium
fetch --nohooks chromium
cd src
git checkout main
gclient sync

# Apply Amnesia patches
cd ..
./scripts/apply_amnesia_patches.sh

# Configure
mkdir -p out/Default
gn gen out/Default --args="$(cat ../AmnesiaBrowser/build_configs/linux/args.gn)"

# Build
autoninja -C out/Default amnesia_browser
```

### macOS Build
Similar to Linux, but use `build_configs/mac/args.gn` and ensure Xcode command line tools are installed.

### Windows Build
Use `build_configs/windows/args.gn` and Visual Studio 2022.

## Architecture Overview

See `docs/architecture.md` for detailed architecture documentation.

## Contributing

See `CONTRIBUTING.md` for contribution guidelines.

## Security

See `docs/security_audit.md` for security audit results and threat model.
