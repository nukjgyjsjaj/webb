#include <jni.h>
#include <string>
#include <vector>
#include <map>
#include <random>
#include <chrono>
#include <mutex>

namespace amnesia {

class AntiFingerprintEngine {
public:
    static AntiFingerprintEngine& GetInstance();

    void Initialize();
    void Shutdown();

    std::string SpoofUserAgent();
    std::string SpoofScreenResolution();
    std::string SpoofLanguage();
    std::string SpoofTimezone();

    void ApplySpoofing();

private:
    AntiFingerprintEngine();
    ~AntiFingerprintEngine();

    AntiFingerprintEngine(const AntiFingerprintEngine&) = delete;
    AntiFingerprintEngine& operator=(const AntiFingerprintEngine&) = delete;

    uint64_t GenerateSeed();
    std::string SelectRandomFromList(const std::vector<std::string>& list, uint64_t seed) const;

    std::mt19937_64 rng_;
    uint64_t session_seed_;
    bool initialized_;
    mutable std::mutex mutex_;

    std::vector<std::string> user_agents_;
    std::vector<std::string> screen_resolutions_;
    std::vector<std::string> languages_;
    std::vector<std::string> timezones_;
};

AntiFingerprintEngine& AntiFingerprintEngine::GetInstance() {
    static AntiFingerprintEngine instance;
    return instance;
}

AntiFingerprintEngine::AntiFingerprintEngine()
    : initialized_(false)
    , session_seed_(0)
{
    std::random_device rd;
    rng_.seed(rd());

    user_agents_ = {
        "Mozilla/5.0 (Linux; Android 14; Pixel 8 Pro) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/125.0.0.0 Mobile Safari/537.36",
        "Mozilla/5.0 (Linux; Android 14; SM-S918B) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/125.0.0.0 Mobile Safari/537.36",
        "Mozilla/5.0 (Linux; Android 13; Pixel 7) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/125.0.0.0 Mobile Safari/537.36",
        "Mozilla/5.0 (Linux; Android 14; SM-S901B) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/124.0.0.0 Mobile Safari/537.36",
        "Mozilla/5.0 (Linux; Android 14; V2304A) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/125.0.0.0 Mobile Safari/537.36"
    };

    screen_resolutions_ = {
        "1440x3200",
        "1080x2340",
        "1080x2400",
        "1440x3120",
        "1080x2160"
    };

    languages_ = {
        "en-US",
        "en-GB",
        "de-DE",
        "fr-FR",
        "ja-JP",
        "ar-SA",
        "zh-CN",
        "ru-RU"
    };

    timezones_ = {
        "America/New_York",
        "Europe/Berlin",
        "Asia/Tokyo",
        "Europe/London",
        "America/Los_Angeles",
        "Asia/Shanghai"
    };
}

AntiFingerprintEngine::~AntiFingerprintEngine() {
    Shutdown();
}

void AntiFingerprintEngine::Initialize() {
    std::lock_guard<std::mutex> lock(mutex_);
    session_seed_ = std::chrono::high_resolution_clock::now().time_since_epoch().count();
    rng_.seed(session_seed_);
    initialized_ = true;
}

void AntiFingerprintEngine::Shutdown() {
    std::lock_guard<std::mutex> lock(mutex_);
    initialized_ = false;
}

std::string AntiFingerprintEngine::SpoofUserAgent() {
    std::lock_guard<std::mutex> lock(mutex_);
    if (!initialized_) return user_agents_[0];
    return SelectRandomFromList(user_agents_, rng_());
}

std::string AntiFingerprintEngine::SpoofScreenResolution() {
    std::lock_guard<std::mutex> lock(mutex_);
    if (!initialized_) return screen_resolutions_[0];
    return SelectRandomFromList(screen_resolutions_, rng_());
}

std::string AntiFingerprintEngine::SpoofLanguage() {
    std::lock_guard<std::mutex> lock(mutex_);
    if (!initialized_) return languages_[0];
    return SelectRandomFromList(languages_, rng_());
}

std::string AntiFingerprintEngine::SpoofTimezone() {
    std::lock_guard<std::mutex> lock(mutex_);
    if (!initialized_) return timezones_[0];
    return SelectRandomFromList(timezones_, rng_());
}

void AntiFingerprintEngine::ApplySpoofing() {
    SpoofUserAgent();
    SpoofScreenResolution();
    SpoofLanguage();
    SpoofTimezone();
}

uint64_t AntiFingerprintEngine::GenerateSeed() {
    return rng_();
}

std::string AntiFingerprintEngine::SelectRandomFromList(const std::vector<std::string>& list, uint64_t seed) const {
    if (list.empty()) return "";
    std::mt19937_64 local_rng(seed);
    std::uniform_int_distribution<size_t> dist(0, list.size() - 1);
    return list[dist(local_rng) % list.size()];
}

}  // namespace amnesia
