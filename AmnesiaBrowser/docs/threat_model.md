# Amnesia Browser - Threat Model

## Assets to Protect
1. User's real IP address
2. User's real location/timezone
3. User's hardware fingerprint (GPU, CPU, etc.)
4. User's browsing history
5. User's cookies and local storage
6. User's session continuity

## Threats

### T1: Browser Fingerprinting (Websites)
- **Capability**: Collect 170+ signals without user consent
- **Impact**: High - Can uniquely identify 99% of users
- **Mitigation**: C++-level spoofing of all 171 signals with deterministic consistency

### T2: IP-Based Tracking
- **Capability**: Link sessions via IP address
- **Impact**: High - Bypasses all other privacy measures
- **Mitigation**: VLESS proxy rotation with auto-rotation on failure

### T3: TLS Fingerprinting
- **Capability**: Identify browser via JA3/JA4 fingerprint
- **Impact**: Medium - Difficult to change without browser modification
- **Mitigation**: uTLS 1.8.1+ with 45+ real browser profiles

### T4: Cross-Session Tracking
- **Capability**: Link multiple browsing sessions
- **Impact**: High - Defeats privacy goals
- **Mitigation**: Complete Amnesia - new identity per session, all data deleted

### T5: Detection of Anti-Fingerprinting Measures
- **Capability**: Detect modified JavaScript APIs
- **Impact**: Medium - Makes user stand out
- **Mitigation**: C++-level patches below JavaScript layer

### T6: Inconsistency Detection
- **Capability**: Detect mismatched signals (e.g., iOS UA + NVIDIA GPU)
- **Impact**: High - Reveals spoofed fingerprints
- **Mitigation**: Deterministic DAG engine, pre-browsing validation

### T7: Memory Forensics
- **Capability**: Recover sensitive data from memory dumps
- **Impact**: Medium - Could recover session data
- **Mitigation**: Memory zeroization, no disk persistence

### T8: Swap/Pagefile Recovery
- **Capability**: Recover deleted data from swap
- **Impact**: Medium - Could recover session traces
- **Mitigation**: Lock memory pages, clear swap on shutdown

### T9: Crash Dump Analysis
- **Capability**: Extract data from crash dumps
- **Impact**: Low-Medium - Depends on OS configuration
- **Mitigation**: Crash handler cleanup, disable crash dumps

### T10: Timing-Based Fingerprinting
- **Capability**: Identify users via timing patterns
- **Impact**: Medium - Behavioral biometrics
- **Mitigation**: Deterministic timing noise (future enhancement)

## Assumptions
1. OS-level adversaries can perform memory forensics
2. Network adversaries can monitor traffic patterns
3. Websites can run arbitrary JavaScript
4. Users may have limited technical knowledge
5. Some websites may block known privacy-focused browsers

## Out of Scope
1. Physical device seizure (user must protect their hardware)
2. Compromised OS/kernel (assumes trusted OS)
3. Quantum computing attacks (not imminent)
4. Side-channel attacks (acoustic, electromagnetic, power)
