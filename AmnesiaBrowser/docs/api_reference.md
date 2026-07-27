# API Reference - Amnesia Browser Internal APIs

## Session Manager API

```cpp
class SessionManager {
public:
    // Singleton access
    static SessionManager& GetInstance();

    // Session lifecycle
    bool CreateSession();
    void DestroySession();
    bool IsSessionActive() const;

    // Seed management
    uint64_t GetCurrentSeed() const;
    uint64_t GenerateSeed();

    // Observer pattern
    void AddObserver(SessionObserver* observer);
    void RemoveObserver(SessionObserver* observer);

    // Security
    void ZeroizeSeed();
    bool IsSeedZeroized() const;

private:
    uint64_t session_seed_;
    std::vector<SessionObserver*> observers_;
    bool session_active_;
};
```

## Fingerprint Generator API

```cpp
class FingerprintGenerator {
public:
    struct Fingerprint {
        uint64_t session_id;
        std::string user_agent;
        std::string platform;
        std::string oscpu;
        std::string timezone;
        std::string locale;
        int screen_width;
        int screen_height;
        int device_pixel_ratio;
        int hardware_concurrency;
        int device_memory;
        std::string webgl_vendor;
        std::string webgl_renderer;
        std::string audio_platform;
        // ... 170+ fields
    };

    Fingerprint Generate(uint64_t seed);
    bool ValidateConsistency(const Fingerprint& fp) const;
    void RegenerateSignal(Fingerprint& fp, SignalType type);
    void RegenerateAll(Fingerprint& fp);

private:
    std::mt19937_64 rng_;
    DeviceProfilesDB device_profiles_;
    BrowserProfilesDB browser_profiles_;
};
```

## Cleanup System API

```cpp
class CleanupSystem {
public:
    static CleanupSystem& GetInstance();

    bool Initialize();
    void Shutdown();

    bool CleanupSession(const std::string& session_id);
    bool CleanupAllSessions();
    bool CleanupEphemeralStorage();
    bool CleanupCache();
    bool CleanupSwap();

    void RegisterShutdownCallback(ShutdownCallback callback);

private:
    std::vector<ShutdownCallback> shutdown_callbacks_;
};
```

## TLS Manager API

```cpp
class TLSManager {
public:
    struct BrowserProfile {
        std::string name;
        uint16_t tls_min_version;
        uint16_t tls_max_version;
        std::vector<uint16_t> cipher_suites;
        std::vector<uint16_t> extensions;
        std::vector<uint16_t> curves;
        std::map<uint16_t, uint32_t> http2_settings;
    };

    bool Initialize();
    bool ConfigureConnection(Connection* conn, const SessionData& session);
    BrowserProfile SelectProfile(uint64_t seed) const;
    std::vector<BrowserProfile> GetAvailableProfiles() const;

private:
    std::vector<BrowserProfile> profiles_;
    BrowserProfile last_successful_profile_;
};
```

## VLESS Manager API

```cpp
class VLESSManager {
public:
    struct VlessLink {
        std::string uuid;
        std::string server;
        uint16_t port;
        std::string transport;  // ws, tcp, reality
        std::string security;   // tls, reality, none
        std::string path;
        std::string host;
        int latency_ms;
        bool valid;
    };

    bool Initialize();
    bool UpdateProxyList();
    std::string GetActiveLink();
    bool RotateLink();
    bool TestLink(const std::string& link, int timeout_seconds);
    void SetLink(const std::string& link);

private:
    std::vector<VlessLink> valid_links_;
    std::string current_link_;
    std::chrono::steady_clock::time_point last_update_;
};
```

## Consistency Engine API

```cpp
class ConsistencyEngine {
public:
    enum class InconsistencyLevel {
        NONE,
        MINOR,      // 1-2 inconsistencies
        MAJOR       // >2 inconsistencies
    };

    struct ValidationResult {
        InconsistencyLevel level;
        std::vector<std::string> inconsistencies;
        bool passed;
    };

    ValidationResult Validate(const Fingerprint& fp);
    bool Repair(Fingerprint& fp);
    bool RegenerateAll(Fingerprint& fp);

private:
    RuleEngine rule_engine_;
    DeviceProfilesDB device_profiles_;
};
```

## GeoIP API

```cpp
class GeoIPDatabase {
public:
    struct GeoInfo {
        std::string country_code;
        std::string country_name;
        std::string region;
        std::string city;
        double latitude;
        double longitude;
        std::string timezone;
        uint32_t asn;
        std::string isp;
    };

    bool LoadDatabase(const std::string& city_db_path, const std::string& asn_db_path);
    GeoInfo Lookup(const std::string& ip_address) const;
    std::string GetTimezoneForCountry(const std::string& country_code) const;

private:
    // MMDB reader instances
    std::unique_ptr<MMDB_s> city_db_;
    std::unique_ptr<MMDB_s> asn_db_;
};
```

## Deterministic Noise API

```cpp
class DeterministicNoise {
public:
    static uint8_t Apply(uint8_t original, uint64_t seed);
    static std::string Apply(const std::string& original, uint64_t seed);
    static std::vector<uint8_t> Apply(const std::vector<uint8_t>& original, uint64_t seed);

private:
    static uint64_t Mix(uint64_t h);
};
```
