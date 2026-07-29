#ifndef AMNESIA_TLS_JA3_CONFIG_H
#define AMNESIA_TLS_JA3_CONFIG_H

#include <string>
#include <vector>
#include <cstdint>
#include <mutex>

namespace amnesia {

struct JA3Config {
    std::string version;
    std::vector<uint16_t> cipher_suites;
    std::vector<uint16_t> extensions;
    std::vector<uint8_t> supported_groups;
    std::vector<uint8_t> ec_point_formats;
    std::string signature_algorithms;
    bool grease_enabled;
};

struct JA4Config {
    std::string tls_version;
    std::string cipher;
    std::string extension;
    std::string algorithm;
    std::string group;
    std::string signature;
    std::string key_share;
};

class JA3ConfigManager {
public:
    static JA3ConfigManager& GetInstance();

    bool Initialize();
    void Shutdown();

    JA3Config GetRandomJA3Config(uint64_t seed);
    JA3Config GetChromeDefaultJA3() const;
    JA4Config GetRandomJA4Config(uint64_t seed);
    std::string GenerateJA3String(const JA3Config& config) const;
    std::string GenerateJA4String(const JA4Config& config) const;

    std::vector<JA3Config> GetPresetConfigs() const;

private:
    JA3ConfigManager();
    ~JA3ConfigManager();

    JA3ConfigManager(const JA3ConfigManager&) = delete;
    JA3ConfigManager& operator=(const JA3ConfigManager&) = delete;

    uint64_t NextU64();

    mutable std::mt19937_64 rng_;
    std::vector<JA3Config> presets_;
    mutable std::mutex mutex_;
    bool initialized_;
};

}  // namespace amnesia

#endif  // AMNESIA_TLS_JA3_CONFIG_H
