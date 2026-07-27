# Device Profiles Database

## Overview
Database of 100+ real device profiles for consistent fingerprint generation.

## Format
Each device profile includes:
- Device model name
- OS and version
- Browser and version
- Screen resolution(s)
- Device pixel ratio
- GPU vendor and renderer
- CPU cores and threads
- RAM memory
- Form factor (Desktop/Laptop/Tablet/Mobile)

## Sources
- GSMArena device database
- DeviceAtlas
- Manual collection from real devices
- Public benchmark datasets

## Structure
```
DeviceProfile {
    string model_name;
    string os_name;
    string os_version;
    string browser_name;
    string browser_version;
    vector<int> resolutions;  // width x height
    vector<int> dpis;  // device pixel ratios
    string gpu_vendor;
    string gpu_renderer;
    int cpu_cores;
    int cpu_threads;
    int ram_gb;
    string form_factor;
}
```

## Usage
```cpp
DeviceProfilesDB& db = DeviceProfilesDB::GetInstance();
DeviceProfile profile = db.GetRandom(seed);
```

## Updates
Profiles are updated automatically from trusted sources on a monthly basis.
See `src/fingerprint/device_profiles/` for implementation.
