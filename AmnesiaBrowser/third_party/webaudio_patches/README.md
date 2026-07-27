# WebAudio Patches

## Overview
Patches for the WebAudio module to apply deterministic noise and prevent audio fingerprinting.

## Files
- webaudio_patches.cc - Main implementation
- audio_context_patches.cc - AudioContext-specific patches
- audio_buffer_patches.cc - AudioBuffer-specific patches

## Key Modifications
1. Deterministic noise in AnalyserNode data
2. Spoofed audio platform information
3. Consistent audio fingerprint across sessions
