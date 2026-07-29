#include "ja3_config.h"
#include <cstdlib>
#include <sstream>
#include <iomanip>

namespace amnesia {

JA3ConfigManager& JA3ConfigManager::GetInstance() {
    static JA3ConfigManager instance;
    return instance;
}

JA3ConfigManager::JA3ConfigManager() : initialized_(false) {
    std::random_device rd;
    rng_.seed(rd());
}

JA3ConfigManager::~JA3ConfigManager() {
    Shutdown();
}

uint64_t JA3ConfigManager::NextU64() {
    return rng_();
}

bool JA3ConfigManager::Initialize() {
    std::lock_guard<std::mutex> lock(mutex_);
    if (initialized_) return true;

    presets_.clear();

    // Chrome 125 default JA3 preset
    JA3Config chrome_default;
    chrome_default.version = "772";
    chrome_default.cipher_suites = {0x1301, 0x1302, 0x1303, 0xc02b, 0xc02f, 0xc02c, 0xc030, 0xcca9, 0xcca8, 0xc013, 0xc014, 0x009c, 0x009d, 0x002f, 0x0035};
    chrome_default.extensions = {0x0000, 0x000b, 0x000a, 0x000d, 0x001b, 0x0017, 0x0023, 0x0010, 0x0012, 0x0005, 0x0015, 0xff01};
    chrome_default.supported_groups = {0x0010, 0x001e, 0x001d, 0x000d};
    chrome_default.ec_point_formats = {0x00};
    chrome_default.signature_algorithms = "0x0403,0x0503,0x0603";
    chrome_default.grease_enabled = false;
    presets_.push_back(chrome_default);

    initialized_ = true;
    return true;
}

void JA3ConfigManager::Shutdown() {
    std::lock_guard<std::mutex> lock(mutex_);
    initialized_ = false;
    presets_.clear();
}

JA3Config JA3ConfigManager::GetRandomJA3Config(uint64_t seed) {
    std::lock_guard<std::mutex> lock(mutex_);
    std::mt19937_64 local_rng(seed);
    std::uniform_int_distribution<size_t> dist(0, presets_.size() - 1);
    return presets_[dist(local_rng)];
}

JA3Config JA3ConfigManager::GetChromeDefaultJA3() const {
    if (!presets_.empty()) return presets_[0];
    return JA3Config();
}

JA4Config JA3ConfigManager::GetRandomJA4Config(uint64_t seed) {
    std::lock_guard<std::mutex> lock(mutex_);
    std::mt19937_64 local_rng(seed);
    std::uniform_int_distribution<int> dist(0, 5);

    JA4Config config;
    config.tls_version = (dist(local_rng) % 2 == 0) ? "tls13" : "tls12";
    config.cipher = "03,032";
    config.extension = "01,01";
    config.algorithm = "01";
    config.group = "03";
    config.signature = "01";
    config.key_share = "01";
    return config;
}

std::string JA3ConfigManager::GenerateJA3String(const JA3Config& config) const {
    std::stringstream ss;

    ss << config.version << ",";

    for (size_t i = 0; i < config.cipher_suites.size(); ++i) {
        if (i > 0) ss << "-";
        ss << std::hex << config.cipher_suites[i];
    }

    ss << ",";

    for (size_t i = 0; i < config.extensions.size(); ++i) {
        if (i > 0) ss << "-";
        ss << std::hex << config.extensions[i];
    }

    ss << ",";

    for (size_t i = 0; i < config.supported_groups.size(); ++i) {
        if (i > 0) ss << "-";
        ss << std::hex << static_cast<int>(config.supported_groups[i]);
    }

    ss << "," << std::hex << static_cast<int>(config.ec_point_formats[0]);
    return ss.str();
}

std::string JA3ConfigManager::GenerateJA4String(const JA4Config& config) const {
    std::stringstream ss;
    ss << config.tls_version << ","
       << config.cipher << ","
       << config.extension << ","
       << config.algorithm << ","
       << config.group << ","
       << config.signature << ","
       << config.key_share;
    return ss.str();
}

std::vector<JA3Config> JA3ConfigManager::GetPresetConfigs() const {
    return presets_;
}

}  // namespace amnesia
