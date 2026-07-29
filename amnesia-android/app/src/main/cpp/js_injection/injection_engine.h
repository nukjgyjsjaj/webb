#ifndef AMNESIA_JS_INJECTION_INJECTION_ENGINE_H
#define AMNESIA_JS_INJECTION_INJECTION_ENGINE_H

#include <string>
#include <vector>
#include <cstdint>
#include <mutex>

namespace amnesia {

struct FingerprintProfile;

class InjectionEngine {
public:
    static InjectionEngine& GetInstance();

    bool Initialize();
    void Shutdown();

    std::string BuildInjectionScript(const FingerprintProfile& profile);
    std::string GetAPIShieldBundle() const;
    std::string GetCanvasNoiseScript(double amplitude) const;
    std::string GetWebGLSpoofScript(const char* renderer, const char* vendor) const;
    std::string GetAudioNoiseScript(double noise_db) const;
    std::string GetNavigatorSpoofScript(const FingerprintProfile& profile) const;
    std::string GetScreenSpoofScript(const FingerprintProfile& profile) const;
    std::string GetWebRTCLeakPreventionScript() const;
    std::string GetTimezoneSpoofScript(const char* timezone, int offset_minutes) const;

    bool InjectIntoWebView(JNIEnv* env, jobject webview, const std::string& script);

private:
    InjectionEngine();
    ~InjectionEngine();

    InjectionEngine(const InjectionEngine&) = delete;
    InjectionEngine& operator=(const InjectionEngine&) = delete;

    std::string BuildNavigatorOverrides(const FingerprintProfile& profile) const;
    std::string BuildScreenOverrides(const FingerprintProfile& profile) const;
    std::string BuildPerformanceOverrides() const;
    std::string BuildDeviceOverrides(const FingerprintProfile& profile) const;
    std::string BuildStorageOverrides(const FingerprintProfile& profile) const;
    std::string BuildHttpHeaderOverrides(const FingerprintProfile& profile) const;

    mutable std::mutex mutex_;
    bool initialized_;
};

}  // namespace amnesia

#endif  // AMNESIA_JS_INJECTION_INJECTION_ENGINE_H
