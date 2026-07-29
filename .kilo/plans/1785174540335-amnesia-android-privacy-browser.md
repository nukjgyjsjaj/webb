# Amnesia Android Browser — Implementation Plan
## WebView Base + Deep NDK Privacy Core

---

## Goal
Build a privacy-hardened Android browser APK (`com.amnesia.browser`) that achieves the full security objectives from the research paper:
- 170+ fingerprint signals neutralized
- Complete amnesia on session close
- Consistent fingerprint within a session
- All websites work without breakage
- Zero persistent storage traces

**Architecture decision:** Android WebView base + NDK-managed privacy core + mandatory JS injection layer. This achieves C++-level control equivalent to engine patches without needing to fork Chromium/Gecko.

---

## Current State (Verified)
- Project structure: `amnesia-android/` with Gradle + CMake + NDK
- 4 Java files (MainActivity, AmnesiaEngine, AmnesiaEngineService, AmnesiaWebView)
- 6 C++ modules (browser_engine, anti_fp, tls, vless, session, cleanup)
- Build configs reviewed and all path/syntax issues fixed
- 19 JNI functions mapped 1:1 between Java and C++
- All necessary files present; only missing pieces are the deep implementation

---

## Implementation Phases

### Phase 1: Core NDK Privacy Engine (Highest Priority)
Build the C++ layer that manages session lifecycle, seed generation, and cleanup.

**Tasks:**
1. Deepen `browser_engine.cpp`:
   - Add session seed management
   - Add fingerprint value registry
   - Add JS injection trigger API
2. Enhance `session_manager.cpp`:
   - 128-bit session ID generation
   - Session state machine (active/expired/destroyed)
   - Seed propagation to all modules
3. Implement `cleanup_system.cpp`:
   - Clear WebView data stores (cookies, cache, localStorage, IndexedDB)
   - Zero RAM buffers on session end
   - File system cleanup for ephemeral directories
4. Implement `amnesia_init.cpp`:
   - Master initialization that seeds all subsystems
   - Guaranteed cleanup on process termination
   - Android lifecycle hooks via JNI

### Phase 2: Anti-Fingerprinting Engine (C++ Core)
Generate consistent, realistic fingerprint values from the session seed.

**Tasks:**
1. Expand `anti_fingerprint.cpp` (already has skeleton):
   - Add all 170+ signal generators
   - User-Agent, platform, language, timezone (done)
   - Screen resolution, DPR, color depth
   - Hardware concurrency, device memory
   - Timezone offset (UTC enforcement)
   - Font list spoofing (return common font set)
   - Canvas: return consistent noised hash instead of raw toDataURL
   - WebGL: return spoofed renderer info + noised outputs
   - AudioContext: return noised frequency data
   - Navigator properties: override all getters
   - Performance API: noised timing values
   - Battery, Bluetooth, USB, MIDI: return undefined/empty
   - Device orientation/motion: return neutral values
2. Add `anti_fp/fingerprint_profile.h/.cpp`:
   - Struct holding all 170+ fingerprint values
   - Generator from PRNG seed
   - JSON serialization for JS injection

### Phase 3: Mandatory JS Injection Layer
The JS layer is the "deep integration" equivalent to engine patches. It runs before any page script and overrides APIs transparently.

**Tasks:**
1. Create `js_injection/injection_engine.cpp`:
   - Build a complete JS bundle string from C++ (loaded from fingerprint profile)
   - Inject into every page load via WebView.evaluateJavascript() at didCommitProvisionalFrame
2. Implement APIShield.js (content script equivalent):
   - Override navigator.* properties
   - Override screen.* properties
   - Override window.* properties
   - Override HTMLCanvasElement.prototype.toDataURL/toBlob
   - Override WebGLRenderingContext.getParameter
   - Override AudioContext.createAnalyser/getByteFrequencyData
   - Override performance.now() with noised timing
   - Override Date()/Date.now() with timezone + noised values
   - Disable/suppress: WebUSB, WebBluetooth, WebMIDI, Sensor APIs
   - Override RTCPeerConnection for WebRTC leak prevention
   - Override document.referrer behavior
   - Block font enumeration APIs (measureText normalization)
3. Wire JS injection into AmnesiaWebView.java:
   - Override didCommitProvisionalFrame in WebViewClient
   - Auto-inject on every navigation
   - Store inject script in NDK to prevent tampering

### Phase 4: VLESS/Xray Integration (NDK-Managed Proxy)
Full proxy management at native level, invisible to the Java layer.

**Tasks:**
1. Expand `vless_proxy.cpp`:
   - Full link parser (UUID, server, port, transport, security)
   - Link validation with timeout-based testing
   - Auto-rotation on failure
   - GitHub repo fetcher (raw links from vless proxy repos)
2. Create `vless/inbound_controller.cpp`:
   - Manage Xray-core process lifecycle
   - Dynamic config generation
   - SOCKS5 local port management
3. Create `vless/outbound_selector.cpp`:
   - Link scoring based on latency/reliability
   - Per-session link selection
4. Wire into `network_security_config.xml` for cleartext proxy

### Phase 5: TLS/JA3/JA4 Spoofing (Network Layer)
Control TLS fingerprint at the network level.

**Tasks:**
1. Expand `tls_spoof.cpp`:
   - Dynamic cipher suite ordering
   - Extension list customization
   - JA3 hash generation from active config
   - JA4 generation
2. Create `tls/ja3_config.h`:
   - Configurable JA3/JA4 templates
   - Per-session selection
3. Integrate with OkHttp in Java layer:
   - OkHttp interceptor for custom TLS socket factory
   - Pass JA3 config from NDK to Java

### Phase 6: Ephemeral Storage & Data Isolation
Ensure zero persistent traces after session ends.

**Tasks:**
1. Create `storage/ephemeral_storage.cpp`:
   - RAM-backed in-memory storage (primary)
   - Encrypted disk fallback (secondary, AES-256-GCM)
   - Automatic purging on session destroy
2. Create `storage/cookie_manager.cpp`:
   - Session-only cookies
   - No persistent cookie jars
   - Isolated per-session cookie stores
3. Wire into AmnesiaEngineService:
   - Service owns the storage directory
   - On service destroy → full cleanup

### Phase 7: Security Hardening & Anti-Tampering
Lock down the browser to prevent bypasses.

**Tasks:**
1. Harden WebView settings in AmnesiaWebView.java:
   - Disable all unnecessary APIs at WebSettings level
   - Set safe defaults for geolocation, camera, microphone
   - Disable file access, form data, passwords
   - Force no-cache mode
   - Block mixed content
2. Add WebView anti-tampering in C++:
   - Detect and prevent debugger attachment
   - Verify injection script integrity
   - Detect Xposed/Frida hooks
3. Implement crash handler:
   - Ensure cleanup runs even on crash
   - Signal handler for SIGSEGV, SIGABRT
4. Add deploy-time certificate pinning (optional, Phase 7 stretch)

### Phase 8: UI Polish & User Controls
Make it usable and transparent.

**Tasks:**
1. Add per-site toggle in AmnesiaWebView:
   - Allow sites to request more permissions (camera, etc.)
   - Visual indicator when privacy features are active
2. Add persistent session option:
   - User can mark sites as "keep logged in"
   - Isolated persistent storage for those sites
3. Statistics dashboard:
   - Show blocked fingerprint attempts
   - Show current fingerprint profile
   - Show proxy status/latency

---

## Module Dependency Graph

```
amnesia_init (master)
├── session_manager (seed generator)
│   ├── browser_engine (orchestrator)
│   └── cleanup_system
├── anti_fp (fingerprint profile generator)
│   └── fingerprint_profile
├── vless_proxy
│   ├── inbound_controller
│   └── outbound_selector
├── tls_spoof
│   └── ja3_config
└── ephemeral_storage
    └── cookie_manager
```

All leaf modules depend on `session_manager` for the session seed.

---

## Validation Plan

### Build Validation
- [ ] `./gradlew assembleDebug` produces valid APK
- [ ] APK installs on arm64-v8a, armeabi-v7a, x86_64
- [ ] No ProGuard/R8 warnings
- [ ] Native library `libamnesia_engine.so` loads without errors
- [ ] All 19 JNI functions resolve at runtime

### Privacy Functionality Tests
- [ ] Session seed changes on app restart
- [ ] All fingerprint values consistent within a session
- [ ] Fingerprint values change between sessions
- [ ] No data persists after app close (cookies, cache, localStorage, IndexedDB)
- [ ] WebRTC leak test: local IP not exposed to websites
- [ ] Canvas fingerprint: different hash each session, consistent within session
- [ ] WebGL renderer: spoofed value returned
- [ ] User-Agent: spoofed value returned
- [ ] Timezone: UTC enforced, not detected as real timezone
- [ ] JA3/JA4: configurable values sent in TLS ClientHello
- [ ] VLESS proxy: traffic routes through local SOCKS5
- [ ] Link rotation: automatic on proxy failure

### Website Compatibility Tests
- [ ] google.com loads and functions
- [ ] youtube.com loads and plays video
- [ ] github.com loads and functions
- [ ] Cloudflare-protected sites pass challenge
- [ ] Banking site (known strict fingerprint checker) passes
- [ ] Social media sites (Facebook, Twitter/X) function
- [ ] CAPTCHA sites function
- [ ] Sites requiring geolocation: can grant temporary permission

### Security Edge Cases
- [ ] App killed mid-session: cleanup runs on next start
- [ ] Network disconnect during proxy use: graceful fallback
- [ ] Invalid VLESS link: auto-rotation to valid link
- [ ] Memory pressure: ephemeral storage handles low memory
- [ ] Multiple rapid session restarts: no data leakage between sessions

---

## Implementation Sequence (Strict Order)

1. Fix remaining build issues in current files
2. Implement `fingerprint_profile.h/.cpp`
3. Implement `js_injection/injection_engine.cpp` + APIShield.js
4. Wire JS injection into WebView lifecycle
5. Implement `epemeral_storage` + `cookie_manager`
6. Implement `cleanup_system.cpp` crash-safe cleanup
7. Expand `anti_fp` with all 170+ signal generators
8. Expand `vless_proxy` + `inbound_controller`
9. Implement `tls` + `ja3_config` + OkHttp integration
10. Security hardening (debugger detection, hook detection)
11. UI polish (statistics dashboard, per-site toggles)
12. Final APK build and test cycle

---

## Key Design Decisions

| Decision | Chosen Approach | Rationale |
|----------|----------------|-----------|
| Rendering base | Android WebView | Only viable path for 100% site compatibility on Android |
| Privacy layer depth | JS injection + NDK orchestration | Equivalent to engine patches without forking Chromium |
| Storage | RAM-only primary, encrypted disk fallback | Zero traces after session + resilience for large data |
| Proxy management | NDK-owned Xray-core process | Invisible to Java layer, robust lifecycle |
| Fingerprint consistency | PRNG seeded from session ID | All values derived from single seed = no contradictions |
| Cleanup trigger | Service destroy + signal handlers | Guaranteed even on crash/kill |
| TLS control | OkHttp custom SSLSocketFactory | Only Java-accessible API for TLS customization on Android |

---

## Risks & Mitigations

| Risk | Mitigation |
|------|-----------|
| WebView version differences across Android versions | Test on API 26, 29, 33, 34; fallback gracefully |
| JS injection detected by anti-bot scripts | Inject in `onPageCommitVisible` before page JS executes; use opaque injection via evaluateJavascript |
| Xray-core binary size | Use minimal build flags; strip symbols |
| OkHttp TLS patch detection | Match real browser cipher suite order from mitmproxy captures |
| Memory pressure clears RAM storage | Transparent fallback to encrypted disk with automatic delete |

---

## Files to Create/Modify

### New Files
- `app/src/main/cpp/js_injection/injection_engine.cpp`
- `app/src/main/cpp/js_injection/api_shield.js` (embedded as string)
- `app/src/main/cpp/anti_fp/fingerprint_profile.h`
- `app/src/main/cpp/anti_fp/fingerprint_profile.cpp`
- `app/src/main/cpp/storage/ephemeral_storage.h`
- `app/src/main/cpp/storage/ephemeral_storage.cpp`
- `app/src/main/cpp/storage/cookie_manager.h`
- `app/src/main/cpp/storage/cookie_manager.cpp`
- `app/src/main/cpp/cleanup/cleanup_system.h`
- `app/src/main/cpp/cleanup/cleanup_system.cpp`
- `app/src/main/cpp/vless/inbound_controller.h`
- `app/src/main/cpp/vless/inbound_controller.cpp`
- `app/src/main/cpp/vless/outbound_selector.h`
- `app/src/main/cpp/vless/outbound_selector.cpp`
- `app/src/main/cpp/tls/ja3_config.h`
- `app/src/main/cpp/tls/ja3_config.cpp`
- `app/src/main/cpp/amnesia_init.h`
- `app/src/main/cpp/amnesia_init.cpp`
- `app/src/main/java/com/amnesia/browser/ApiShieldInjector.java`
- `app/src/main/java/com/amnesia/browser/PrivacyDashboardActivity.java`

### Modified Files
- `app/src/main/cpp/engine/browser_engine.cpp`
- `app/src/main/cpp/anti_fp/anti_fingerprint.cpp`
- `app/src/main/cpp/tls/tls_spoof.cpp`
- `app/src/main/cpp/proxy/vless_proxy.cpp`
- `app/src/main/cpp/session/session_manager.cpp`
- `app/src/main/cpp/cleanup/cleanup.cpp`
- `app/src/main/java/com/amnesia/browser/AmnesiaEngine.java`
- `app/src/main/java/com/amnesia/browser/AmnesiaWebView.java`
- `app/src/main/java/com/amnesia/browser/MainActivity.java`
- `amnesia-android/app/build.gradle`
- `amnesia-android/cpp/CMakeLists.txt`

---

## Out of Scope
- Building a custom rendering engine (not feasible)
- Supporting Android versions below API 26 (Android 8.0)
- Desktop/Linux builds
- Chromium source modifications
- Tor network integration (use VLESS instead as specified)
