# TLS Profiles

## Overview
TLS profile configurations for uTLS integration.

## Files
- chrome_profiles.json - Chrome profiles across platforms
- firefox_profiles.json - Firefox profiles across platforms
- safari_profiles.json - Safari profiles across platforms
- edge_profiles.json - Edge profiles across platforms

## Format
Each profile includes:
```json
{
  "name": "Chrome_120_Windows",
  "tls_version": {"min": "1.2", "max": "1.3"},
  "cipher_suites": [0x1301, 0x1302, ...],
  "extensions": [0x0000, 0x0005, ...],
  "curves": [0x0017, 0x0018, ...],
  "http2_settings": {"window_update": 65536, ...},
  "alpn": ["h2", "http/1.1"]
}
```

## Generation
Profiles can be generated from real traffic captures using:
- tlsfingerprint.io
- ja3er.com
- Wireshark/TShark scripts

## Security
- Profiles are verified against real browser traffic
- Outdated profiles are automatically flagged
- CVE-2026-27017 mitigation included
