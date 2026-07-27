# Canvas Patches

## Overview
Patches for the Canvas module to apply deterministic noise and prevent canvas fingerprinting.

## Files
- canvas_patches.cc - Main implementation
- html_canvas_element_patches.cc - HTMLCanvasElement-specific patches
- canvas_rendering_context_patches.cc - 2D context patches

## Key Modifications
1. Deterministic noise in toDataURL() output
2. Deterministic noise in getImageData() output
3. Spoofed canvas dimensions and capabilities
4. Pixel permutation for fingerprint resistance

## Why Deterministic Noise?
Instead of blocking canvas entirely, we apply consistent noise that:
- Changes the fingerprint value
- Maintains visual quality
- Is deterministic within a session
- Becomes invisible across sessions
