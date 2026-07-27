# Amnesia Browser Blink Patches

## Overview
This directory contains C++ patches for the Blink rendering engine to enable
comprehensive browser fingerprinting resistance.

## Patches Organization

### navigator_patches.cc/h
- Spoofs navigator.userAgent
- Spoofs navigator.platform
- Spoofs navigator.oscpu
- Spoofs Sec-CH-UA headers
- Spoofs True OS Core detection

### screen_patches.cc/h
- Spoofs screen.width/height
- Spoofs window.devicePixelRatio
- Spoofs window.innerWidth/innerHeight
- Spoofs window.outerWidth/outerHeight
- Spoofs screen.orientation
- Spoofs screen.colorDepth

### canvas_patches.cc/h
- Applies deterministic noise to canvas.toDataURL()
- Prevents fingerprinting while maintaining functionality
- Uses session seed for noise generation

### webgl_patches.cc/h
- Spoofs WEBGL_debug_renderer_info
- Spoofs WebGL vendor/renderer strings
- Applies deterministic noise to WebGL outputs

### webaudio_patches.cc/h
- Applies deterministic noise to AudioContext processing
- Spoofs audio platform information
- Ensures consistent audio fingerprints

### header_patches.cc/h
- Spoofs all HTTP request headers
- Spoofs Sec-CH-* Client Hints
- Ensures header consistency with user agent

## Build Integration
These patches are integrated via:
1. GN args: `enable_amnesia_browser = true`
2. Blink's build system patches
3. Chromium's `build/config/compiler/BUILD.gn` modifications

## Maintenance
Patches need to be rebased on each new Chromium version.
See `scripts/update_blink_patches.sh` for automated rebasing.
