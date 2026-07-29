#ifndef AMNESIA_ANTI_FP_FINGERPRINT_PROFILE_H
#define AMNESIA_ANTI_FP_FINGERPRINT_PROFILE_H

#include <cstdint>
#include <string>
#include <vector>
#include <mutex>

namespace amnesia {

constexpr size_t MAX_USER_AGENT_LEN = 512;
constexpr size_t MAX_STRING_LEN = 256;
constexpr size_t FINGERPRINT_HASH_LEN = 64;

struct FingerprintProfile {
    uint64_t session_id;

    // Device & OS (6 signals)
    char user_agent[MAX_USER_AGENT_LEN];
    char platform[MAX_STRING_LEN];
    char oscpu[MAX_STRING_LEN];
    char browser_name[MAX_STRING_LEN];
    char browser_version[MAX_STRING_LEN];
    char browser_build[MAX_STRING_LEN];

    // Hardware (22 signals - subset)
    int hardware_concurrency;
    int device_memory_gb;
    int cpu_cores;
    char cpu_architecture[64];
    char cpu_bitness[16];
    bool touchscreen_support;
    bool mouse_support;

    // Screen & Display (12 signals - subset)
    int screen_width;
    int screen_height;
    int screen_avail_width;
    int screen_avail_height;
    int color_depth;
    int pixel_depth;
    double device_pixel_ratio;
    int outer_width;
    int outer_height;
    int inner_width;
    int inner_height;
    int screen_orientation_type;

    // Time & Timezone (6 signals - subset)
    char timezone_name[64];
    int timezone_offset_minutes;
    int timezone_dst;
    char locale_language[16];
    char locale_region[16];
    bool date_override;

    // Network (subset for JS layer)
    char connection_type[32];
    bool effective_type_4g;
    int downlink_mbps;
    int rtt_ms;

    // Canvas/WebGL/Audio spoofing config
    bool canvas_noise_enabled;
    double canvas_noise_amplitude;
    bool webgl_spoof_enabled;
    char webgl_renderer[MAX_STRING_LEN];
    char webgl_vendor[MAX_STRING_LEN];
    bool webgpu_spoof_enabled;
    bool audiocontext_noise_enabled;
    double audio_noise_db;

    // Sensors/Peripherals
    bool webusb_enabled;
    bool webbluetooth_enabled;
    bool webmidi_enabled;
    bool bluetooth_enabled;
    int media_devices_count;
    bool battery_api_enabled;
    bool geolocation_api_enabled;
    bool gamepad_api_enabled;

    // Font enumeration
    char font_list[4096];
    bool font_spoof_enabled;

    // Performance timing
    double perf_noise_factor;
    bool performance_spoof_enabled;

    // Storage policy
    bool cookies_enabled;
    bool localStorage_enabled;
    bool sessionStorage_enabled;
    bool indexeddb_enabled;
    bool cache_api_enabled;
    bool service_worker_enabled;

    // HTTP headers (for OkHttp layer)
    char accept_language[64];
    char sec_ch_ua[MAX_STRING_LEN];
    char sec_ch_ua_mobile[MAX_STRING_LEN];
    char sec_ch_ua_platform[MAX_STRING_LEN];

    // Fingerprint consistency hash
    char session_hash[FINGERPRINT_HASH_LEN];
};

class FingerprintProfileGenerator {
public:
    static FingerprintProfileGenerator& GetInstance();

    bool Initialize();
    void Shutdown();

    FingerprintProfile Generate(uint64_t session_seed);
    std::string GenerateSessionHash(const FingerprintProfile& profile);
    std::string ToJson(const FingerprintProfile& profile);

    const FingerprintProfile& GetCurrentProfile() const;

private:
    FingerprintProfileGenerator();
    ~FingerprintProfileGenerator();

    FingerprintProfileGenerator(const FingerprintProfileGenerator&) = delete;
    FingerprintProfileGenerator& operator=(const FingerprintProfileGenerator&) = delete;

    uint64_t rng_seed_;
    FingerprintProfile current_profile_;
    std::mutex mutex_;
    bool initialized_;
};

}  // namespace amnesia

#endif  // AMNESIA_ANTI_FP_FINGERPRINT_PROFILE_H
