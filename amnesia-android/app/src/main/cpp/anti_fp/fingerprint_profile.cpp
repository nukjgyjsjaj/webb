#include "fingerprint_profile.h"
#include <cstring>
#include <cstdlib>
#include <sstream>
#include <iomanip>
#include <random>
#include <chrono>

namespace amnesia {

FingerprintProfileGenerator& FingerprintProfileGenerator::GetInstance() {
    static FingerprintProfileGenerator instance;
    return instance;
}

FingerprintProfileGenerator::FingerprintProfileGenerator() : initialized_(false) {
    std::random_device rd;
    rng_seed_ = rd();
}

FingerprintProfileGenerator::~FingerprintProfileGenerator() {
    Shutdown();
}

bool FingerprintProfileGenerator::Initialize() {
    std::lock_guard<std::mutex> lock(mutex_);
    if (initialized_) return true;
    initialized_ = true;
    return true;
}

void FingerprintProfileGenerator::Shutdown() {
    std::lock_guard<std::mutex> lock(mutex_);
    initialized_ = false;
}

static uint64_t NextU64(uint64_t& seed) {
    seed = seed * 6364136223846793005ULL + 1442695040888963407ULL;
    return seed;
}

static std::string SelectFrom(const std::vector<std::string>& list, uint64_t& seed) {
    if (list.empty()) return "";
    size_t idx = NextU64(seed) % list.size();
    return list[idx];
}

FingerprintProfile FingerprintProfileGenerator::Generate(uint64_t session_seed) {
    std::lock_guard<std::mutex> lock(mutex_);
    rng_seed_ = session_seed;
    FingerprintProfile fp{};
    fp.session_id = session_seed;

    static const std::vector<std::string> kUserAgents = {
        "Mozilla/5.0 (Linux; Android 14; Pixel 8 Pro) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/125.0.0.0 Mobile Safari/537.36",
        "Mozilla/5.0 (Linux; Android 14; SM-S918B) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/125.0.0.0 Mobile Safari/537.36",
        "Mozilla/5.0 (Linux; Android 13; Pixel 7) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/125.0.0.0 Mobile Safari/537.36",
        "Mozilla/5.0 (Linux; Android 14; SM-S901B) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/124.0.0.0 Mobile Safari/537.36",
        "Mozilla/5.0 (Linux; Android 14; V2304A) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/125.0.0.0 Mobile Safari/537.36"
    };

    static const std::vector<std::string> kPlatforms = {"Linux armv8l", "Linux aarch64", "Linux i686", "Linux x86_64", "MacIntel", "Win32"};
    static const std::vector<std::string> kOscpus = {"Linux x86_64", "Linux armv8l", "Linux aarch64", "Windows NT 10.0", "Macintosh"};
    static const std::vector<std::string> kCpuArchs = {"arm", "arm64", "x86", "x86_64"};
    static const std::vector<std::string> kCpuBits = {"64", "32"};
    static const std::vector<std::string> kTimezones = {"America/New_York", "Europe/Berlin", "Asia/Tokyo", "Europe/London", "America/Los_Angeles", "Asia/Shanghai", "UTC", "Europe/Paris", "America/Chicago", "Asia/Dubai"};
    static const std::vector<std::string> kLocalesLang = {"en-US", "en-GB", "de-DE", "fr-FR", "ja-JP", "ar-SA", "zh-CN", "ru-RU", "es-ES", "pt-BR"};
    static const std::vector<std::string> kLocalesRegion = {"US", "GB", "DE", "FR", "JP", "SA", "CN", "RU", "ES", "BR"};
    static const std::vector<std::string> kAccLangs = {"en-US,en;q=0.9", "en-US,en;q=0.9,fr;q=0.8", "de-DE,de;q=0.9,en;q=0.8", "zh-CN,zh;q=0.9,en;q=0.8", "ja-JP,ja;q=0.9,en;q=0.8"};
    static const std::vector<std::string> kResolutions = {"1440x3200", "1080x2340", "1080x2400", "1440x3120", "1080x2160", "768x1600", "720x1480"};
    static const std::vector<std::string> kConns = {"4g", "3g", "2g", "slow-2g"};
    static const std::vector<std::string> kRenderers = {
        "ANGLE (Qualcomm, Adreno (TM) 750, OpenGL ES 3.2)",
        "ANGLE (Qualcomm, Adreno (TM) 740, OpenGL ES 3.2 V12.spec)",
        "SwiftShader",
        "ANGLE (Google, Vulkan 1.3.0)",
        "ANGLE (NVIDIA, NVIDIA GeForce RTX 4090, OpenGL 4.5)",
        "Mesa DRI Intel(R) Arc(TM) A770 Graphics (DG2)"
    };
    static const std::vector<std::string> kVendors = {"Qualcomm", "Google", "NVIDIA Corporation", "Intel Inc.", "ARM", "Apple", "Mesa"};
    static const std::vector<std::string> kFonts = {
        "Roboto,RobotoDraft,Noto Sans,Cantarell,Droid Sans,sans-serif",
        "Roboto,Noto Sans,Noto Sans JP,Noto Sans SC,sans-serif",
        "Segoe UI,Roboto,sans-serif,Noto Sans",
        "SF Pro Text,Roboto,sans-serif"
    };

    // Device & OS
    strncpy(fp.user_agent, SelectFrom(kUserAgents, rng_seed_).c_str(), sizeof(fp.user_agent) - 1);
    strncpy(fp.platform, SelectFrom(kPlatforms, rng_seed_).c_str(), sizeof(fp.platform) - 1);
    strncpy(fp.oscpu, SelectFrom(kOscpus, rng_seed_).c_str(), sizeof(fp.oscpu) - 1);
    strncpy(fp.browser_name, "Google Chrome", sizeof(fp.browser_name) - 1);
    strncpy(fp.browser_version, "125.0.0.0", sizeof(fp.browser_version) - 1);
    snprintf(fp.browser_build, sizeof(fp.browser_build), "%d", 20240615 + (NextU64(rng_seed_) % 30));

    // Hardware
    fp.hardware_concurrency = (NextU64(rng_seed_) % 6) + 4;
    fp.device_memory_gb = (NextU64(rng_seed_) % 8) + 2;
    fp.cpu_cores = fp.hardware_concurrency;
    strncpy(fp.cpu_architecture, SelectFrom(kCpuArchs, rng_seed_).c_str(), sizeof(fp.cpu_architecture) - 1);
    strncpy(fp.cpu_bitness, "64", sizeof(fp.cpu_bitness) - 1);
    fp.touchscreen_support = true;
    fp.mouse_support = (NextU64(rng_seed_) % 3) == 0;

    // Screen & Display
    std::string res = SelectFrom(kResolutions, rng_seed_);
    int w = 1080, h = 2340;
    if (res.find('x') != std::string::npos) {
        sscanf(res.c_str(), "%dx%d", &w, &h);
    }
    fp.screen_width = w;
    fp.screen_height = h;
    fp.screen_avail_width = w - (NextU64(rng_seed_) % 50 + 20);
    fp.screen_avail_height = h - (NextU64(rng_seed_) % 100 + 40);
    fp.color_depth = 24;
    fp.pixel_depth = 32;
    fp.device_pixel_ratio = 2.0 + ((NextU64(rng_seed_) % 30) / 10.0);
    fp.outer_width = fp.screen_avail_width;
    fp.outer_height = fp.screen_avail_height - 100;
    fp.inner_width = fp.outer_width - (NextU64(rng_seed_) % 30);
    fp.inner_height = fp.outer_height - (NextU64(rng_seed_) % 30);
    fp.screen_orientation_type = 0;

    // Time & Timezone
    strncpy(fp.timezone_name, SelectFrom(kTimezones, rng_seed_).c_str(), sizeof(fp.timezone_name) - 1);
    int tz_minutes = (NextU64(rng_seed_) % 720) - 360;
    fp.timezone_offset_minutes = tz_minutes;
    fp.timezone_dst = (NextU64(rng_seed_) % 2);
    strncpy(fp.locale_language, SelectFrom(kLocalesLang, rng_seed_).c_str(), sizeof(fp.locale_language) - 1);
    strncpy(fp.locale_region, SelectFrom(kLocalesRegion, rng_seed_).c_str(), sizeof(fp.locale_region) - 1);

    // Network
    strncpy(fp.connection_type, SelectFrom(kConns, rng_seed_).c_str(), sizeof(fp.connection_type) - 1);
    fp.effective_type_4g = true;
    fp.downlink_mbps = (NextU64(rng_seed_) % 100) + 10;
    fp.rtt_ms = (NextU64(rng_seed_) % 200) + 20;

    // Canvas/WebGL/Audio
    fp.canvas_noise_enabled = true;
    fp.canvas_noise_amplitude = 0.02;
    fp.webgl_spoof_enabled = true;
    strncpy(fp.webgl_renderer, SelectFrom(kRenderers, rng_seed_).c_str(), sizeof(fp.webgl_renderer) - 1);
    strncpy(fp.webgl_vendor, SelectFrom(kVendors, rng_seed_).c_str(), sizeof(fp.webgl_vendor) - 1);
    fp.webgpu_spoof_enabled = true;
    fp.audiocontext_noise_enabled = true;
    fp.audio_noise_db = 0.01;

    // Sensors/Peripherals
    fp.webusb_enabled = false;
    fp.webbluetooth_enabled = false;
    fp.webmidi_enabled = false;
    fp.bluetooth_enabled = (NextU64(rng_seed_) % 3) == 0;
    fp.media_devices_count = 1 + (NextU64(rng_seed_) % 2);
    fp.battery_api_enabled = (NextU64(rng_seed_) % 2) == 0;
    fp.geolocation_api_enabled = true;
    fp.gamepad_api_enabled = false;

    // Font enumeration
    strncpy(fp.font_list, SelectFrom(kFonts, rng_seed_).c_str(), sizeof(fp.font_list) - 1);
    fp.font_spoof_enabled = true;

    // Performance timing
    fp.perf_noise_factor = 0.05;
    fp.performance_spoof_enabled = true;

    // Storage policy (session-only by default)
    fp.cookies_enabled = true;
    fp.localStorage_enabled = true;
    fp.sessionStorage_enabled = true;
    fp.indexeddb_enabled = true;
    fp.cache_api_enabled = false;
    fp.service_worker_enabled = true;

    // HTTP headers
    strncpy(fp.accept_language, SelectFrom(kAccLangs, rng_seed_).c_str(), sizeof(fp.accept_language) - 1);
    snprintf(fp.sec_ch_ua, sizeof(fp.sec_ch_ua), "\"Chromium\";v=\"125\", \"Google Chrome\";v=\"125\"");
    strncpy(fp.sec_ch_ua_mobile, "?1", sizeof(fp.sec_ch_ua_mobile) - 1);
    strncpy(fp.sec_ch_ua_platform, "\"Android\"", sizeof(fp.sec_ch_ua_platform) - 1);

    // Session hash
    std::stringstream hash_ss;
    hash_ss << std::hex << session_seed;
    strncpy(fp.session_hash, hash_ss.str().c_str(), sizeof(fp.session_hash) - 1);

    current_profile_ = fp;
    return fp;
}

std::string FingerprintProfileGenerator::GenerateSessionHash(const FingerprintProfile& profile) {
    return std::string(profile.session_hash);
}

std::string EscapeJson(const char* src, size_t len) {
    std::string out;
    out.reserve(len * 2);
    for (size_t i = 0; i < len; ++i) {
        unsigned char c = src[i];
        if (c == '"') out += "\\\"";
        else if (c == '\\') out += "\\\\";
        else if (c == '\n') out += "\\n";
        else if (c == '\r') out += "\\r";
        else if (c == '\t') out += "\\t";
        else if (c < 0x20) {
            char buf[7];
            snprintf(buf, sizeof(buf), "\\u%04x", c);
            out += buf;
        } else {
            out += c;
        }
    }
    return out;
}

std::string FingerprintProfileGenerator::ToJson(const FingerprintProfile& p) {
    std::stringstream ss;
    ss << "{";
    ss << "\"user_agent\":\"" << EscapeJson(p.user_agent, strlen(p.user_agent)) << "\",";
    ss << "\"platform\":\"" << EscapeJson(p.platform, strlen(p.platform)) << "\",";
    ss << "\"screen_width\":" << p.screen_width << ",";
    ss << "\"screen_height\":" << p.screen_height << ",";
    ss << "\"device_pixel_ratio\":" << std::fixed << std::setprecision(2) << p.device_pixel_ratio << ",";
    ss << "\"hardware_concurrency\":" << p.hardware_concurrency << ",";
    ss << "\"device_memory_gb\":" << p.device_memory_gb << ",";
    ss << "\"timezone_name\":\"" << EscapeJson(p.timezone_name, strlen(p.timezone_name)) << "\",";
    ss << "\"timezone_offset_minutes\":" << p.timezone_offset_minutes << ",";
    ss << "\"locale\":\"" << EscapeJson(std::string(p.locale_language) + "_" + p.locale_region, 64) << "\",";
    ss << "\"webgl_renderer\":\"" << EscapeJson(p.webgl_renderer, strlen(p.webgl_renderer)) << "\",";
    ss << "\"webgl_vendor\":\"" << EscapeJson(p.webgl_vendor, strlen(p.webgl_vendor)) << "\",";
    ss << "\"canvas_noise_amplitude\":" << std::fixed << std::setprecision(3) << p.canvas_noise_amplitude << ",";
    ss << "\"audio_noise_db\":" << std::fixed << std::setprecision(3) << p.audio_noise_db << ",";
    ss << "\"accept_language\":\"" << EscapeJson(p.accept_language, strlen(p.accept_language)) << "\",";
    ss << "\"connection_type\":\"" << EscapeJson(p.connection_type, strlen(p.connection_type)) << "\",";
    ss << "\"downlink_mbps\":" << p.downlink_mbps << ",";
    ss << "\"sec_ch_ua\":\"" << EscapeJson(p.sec_ch_ua, strlen(p.sec_ch_ua)) << "\",";
    ss << "\"sec_ch_ua_mobile\":\"" << EscapeJson(p.sec_ch_ua_mobile, strlen(p.sec_ch_ua_mobile)) << "\",";
    ss << "\"sec_ch_ua_platform\":\"" << EscapeJson(p.sec_ch_ua_platform, strlen(p.sec_ch_ua_platform)) << "\"";
    ss << "}";
    return ss.str();
}

const FingerprintProfile& FingerprintProfileGenerator::GetCurrentProfile() const {
    return current_profile_;
}

}  // namespace amnesia
