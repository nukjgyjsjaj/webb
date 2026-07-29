# Amnesia Android Browser - Build From Scratch

## Status
- **Target**: Android APK only
- **Output**: Single working `app-debug.apk`
- **Base**: Custom C++ browser engine via NDK, not extensions over an existing browser
- **Roadmap**: C++ core modules → Android UI → APK build → 5+ terminal validation

## Current Project Files To Rebuild
- `app/src/main/AndroidManifest.xml`
- `app/src/main/java/com/amnesia/browser/*`
- `app/build.gradle`
- `cpp/**/CMakeLists.txt`
- `resources/**` minimal payloads only

## Constraints
- No desktop leftovers
- No reused Chromium UI bindings
- Every feature must be implemented in-engine or removed
- Final deliverable is an APK

## Needed Modules
1. Android C++ core engine
2. Anti-fingerprinting
3. TLS/JA3/JA4
4. VLESS/Xray proxy
5. Session + cleanup
6. Gradle APK packaging
