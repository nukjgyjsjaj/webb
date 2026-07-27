# Build Instructions - Amnesia Browser

## Prerequisites

### All Platforms
- depot_tools (https://chromium.googlesource.com/chromium/tools/depot_tools.git)
- Python 3.9+
- CMake 3.20+
- Git 2.30+
- 100GB+ free disk space
- 16GB+ RAM
- 8+ CPU cores recommended

### Linux (Ubuntu 22.04)
```bash
sudo apt-get update
sudo apt-get install -y python3 python3-pip git gnupg flex bison gperf \
    build-essential zip curl zlib1g-dev gcc-multilib g++-multilib \
    libc6-dev-i386 lib32z1-dev lib32ncurses5-dev x11proto-core-dev \
    libx11-dev libx11-xcb-dev libxcb1-dev libxcb-util0-dev libxcb-icccm4-dev \
    libxcb-image0-dev libxcb-keysyms1-dev libxcb-randr0-dev libxcb-render0-dev \
    libxcb-shape0-dev libxcb-sync-dev libxcb-xfixes0-dev libxcb-xinerama0-dev \
    libxcb-xkb-dev libxcb-xrm-dev libxcb-xinput-dev libxkbcommon-dev \
    libxkbcommon-x11-dev libwayland-dev libudev-dev libpci-dev \
    libdrm-dev libgbm-dev libasound2-dev libpulse-dev libxss-dev \
    libxtst-dev libnss3-dev libatk1.0-dev libatk-bridge2.0-dev \
    libcups2-dev libdrm-dev libxcomposite-dev libxdamage-dev libxrandr-dev \
    libpango1.0-dev libcairo2-dev libatspi2.0-dev libgtk-3-dev
```

### macOS
- Xcode 14.3+
- Command Line Tools: `xcode-select --install`
- Python 3.9+: `brew install python@3.9`

### Windows
- Visual Studio 2022 with C++ workload
- Windows 10/11 SDK
- Python 3.9+

## Step-by-Step Build

### 1. Clone Repository
```bash
git clone https://github.com/amnesia-browser/amnesia.git
cd amnesia
```

### 2. Setup depot_tools
```bash
git clone https://chromium.googlesource.com/chromium/tools/depot_tools.git ~/depot_tools
export PATH=~/depot_tools:$PATH
echo 'export PATH=~/depot_tools:$PATH' >> ~/.bashrc
```

### 3. Fetch Chromium
```bash
cd AmnesiaBrowser
bash scripts/fetch_chromium.sh
cd chrome
gclient sync
```

### 4. Apply Patches
```bash
cd ..
bash scripts/apply_amnesia_patches.sh
```

### 5. Configure
```bash
bash scripts/configure.sh
```

### 6. Build
```bash
bash scripts/build.sh
```

### 7. Test
```bash
bash scripts/run_unit_tests.sh
bash scripts/run_integration_tests.sh
bash scripts/run_consistency_tests.sh
```

## Build Targets
- `amnesia_browser` - Main browser executable
- `amnesia_browser_unittests` - Unit tests
- `amnesia_browser_integration_tests` - Integration tests
- `consistency_validator` - Standalone consistency validator

## Build Output
- Linux: `chrome/src/out/Default/amnesia_browser`
- macOS: `chrome/src/out/Default/Amnesia Browser.app`
- Windows: `chrome/src/out/Default/amnesia_browser.exe`

## Troubleshooting

### Common Issues
1. **Out of memory**: Increase swap or use fewer parallel jobs: `autoninja -j4`
2. **Disk space**: Clean with `make clean` or `gclient sync --reset`
3. **depot_tools not found**: Ensure PATH is set correctly
4. **GN generation failed**: Check build_configs/*/args.gn for syntax errors

### Getting Help
- Open an issue on GitHub
- Check documentation in `docs/`
- Join our Discord community
