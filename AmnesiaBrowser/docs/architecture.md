# Amnesia Browser - Architecture Documentation

## System Overview

Amnesia Browser is a privacy-focused web browser built on Chromium/Blink with C++-level patches for browser fingerprinting resistance. It implements a "Complete Amnesia" model where every browsing session starts fresh with a new, internally-coherent browser identity.

## Core Components

### 1. Session Manager
- Creates 128-bit random Session ID at startup
- Stores Session ID in memory only (never on disk)
- Coordinates all browser components via Observer Pattern
- Zeroizes memory on shutdown

### 2. Fingerprint Generator
- Deterministic PRNG (Mulberry32/PCG/SplitMix64)
- Generates 170+ fingerprint values from single seed
- All values within natural ranges for the signal
- Produces internally consistent results

### 3. Ephemeral Storage Manager
- RAM-backed or encrypted temporary directory
- All cookie/storage/cache operations routed here
- Complete deletion on session end

### 4. Cleanup System
- Monitors Browser::Shutdown and SIGTERM/SIGINT
- Deletes all cookies, cache, session data
- Zeroizes allocated memory
- Clears swap/pagefile traces
- Works even on crashes

### 5. C++ Engine Patches
Located in:
- `third_party/blink/renderer/core/frame/navigator.cc` - User-Agent, platform
- `third_party/blink/renderer/core/frame/local_dom_window.cc` - Screen, Window
- `third_party/blink/renderer/modules/canvas/` - Canvas fingerprinting
- `third_party/blink/renderer/modules/webgl/` - WebGL/WebGPU
- `third_party/blink/renderer/modules/webaudio/` - AudioContext
- `net/http/http_util.cc` - HTTP headers
- `components/network_session_configurator/` - TLS settings

### 6. TLS Manager
- uTLS 1.8.1+ integration
- 45+ browser profiles (Chrome, Firefox, Safari, Edge)
- JA3/JA4 spoofing
- HTTP/2 settings synchronization

### 7. VLESS Manager
- Fetches proxy lists from 5 GitHub sources (every 15 min)
- Tests 3-5 links for validity
- Auto-rotation on failure
- Xray-core integration with internal API

### 8. Consistency Engine
- Deterministic DAG (48 rules)
- Device profiles database (100+ real devices)
- Pre-browsing validation (4 tests)
- Auto-regeneration on inconsistency

## Data Flow

```
Startup
  ↓
Session Manager creates 128-bit seed
  ↓
┌─────────────┐     ┌─────────────┐     ┌─────────────┐
│ VLESS       │     │ Fingerprint │     │ TLS Profile │
│ Collector   │     │ Generator   │     │ Selector    │
└──────┬──────┘     └──────┬──────┘     └──────┬──────┘
       │                   │                   │
       └───────────────────┼───────────────────┘
                           │
                           ▼
                  ┌──────────────────┐
                  │ Consistency Engine│
                  │ - DAG Rules      │
                  │ - Validation     │
                  └─────────┬────────┘
                            │
                            ▼
                  ┌──────────────────┐
                  │ C++ Engine Patches│
                  │ - Inject values  │
                  └─────────┬────────┘
                            │
                            ▼
                     ┌──────────────┐
                     │ Ready to Browse│
                     └──────────────┘

Browsing Session
  ↓
All traffic routed through VLESS proxy
  ↓
TLS connections use spoofed fingerprints
  ↓
JavaScript APIs return consistent values
  ↓

Shutdown
  ↓
Cleanup System deletes everything
  ↓
Memory zeroized
  ↓
Session ID destroyed
```

## Fingerprint Signals (171 total)

See `notes.md` for complete list organized by category:
1. Device/OS (6)
2. Network/Location (13)
3. Time/Timezone (6)
4. Canvas (3)
5. AudioContext (2)
6. User-Agent & Headers (33)
7. Hardware/Peripherals (22)
8. Interaction (13)
9. Browser Settings (20)
10. Screen/Display (12)
11. Storage/Performance (15)
12. Emerging Vectors (24)

## Consistency Rules (48 DAG Rules)

1. Device Type → OS, Browser
2. OS → Compatible Hardware
3. Browser → TLS Profile, HTTP/2 Settings
4. Proxy IP → Country, Timezone, Language
5. GPU Vendor → Compatible Drivers
6. Screen Resolution → Device Pixel Ratio
7. etc.

Key principle: "Mac UA never lands next to Linux WebGL"
