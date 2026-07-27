# Browser Profiles Library

## Overview
Library of 45+ real browser TLS and HTTP/2 profiles.

## Contents
- JA3/JA4 fingerprint databases
- TLS version matrices
- Cipher suite orderings
- Extension orderings
- Elliptic curve preferences
- HTTP/2 settings frames
- ALPN protocol lists

## Browser Coverage
- Chrome (Windows, macOS, Linux, Android, iOS)
- Firefox (Windows, macOS, Linux, Android, iOS)
- Safari (macOS, iOS)
- Edge (Windows, macOS)

## Sources
- Real-world captures
- uTLS project profiles
- TLS-Chameleon profiles
- Manual analysis of browser traffic

## Structure
```
BrowserProfile {
    string name;
    uint16_t tls_min_version;
    uint16_t tls_max_version;
    vector<uint16_t> cipher_suites;
    vector<uint16_t> extensions;
    vector<uint16_t> curves;
    map<uint16_t, uint32_t> http2_settings;
    vector<string> alpn;
}
```

## Usage
```cpp
BrowserProfilesDB& db = BrowserProfilesDB::GetInstance();
BrowserProfile profile = db.GetRandom(seed);
```

## Updates
Profiles are updated automatically from trusted sources.
See `src/tls/browser_profiles/` for implementation.
