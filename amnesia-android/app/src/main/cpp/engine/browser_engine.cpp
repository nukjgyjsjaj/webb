#include <jni.h>
#include <string>
#include <vector>
#include <map>
#include <random>
#include <chrono>
#include <mutex>
#include <cstdint>
#include <cstring>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <cstdio>
#include <cstdlib>
#include <csignal>
#include <atomic>
#include <filesystem>

#include "session/session_manager.h"
#include "anti_fp/fingerprint_profile.h"
#include "storage/ephemeral_storage.h"
#include "storage/cookie_manager.h"
#include "cleanup/cleanup_system.h"
#include "js_injection/injection_engine.h"
#include "tls/ja3_config.h"
#include "vless/inbound_controller.h"
#include "vless/outbound_selector.h"

namespace fs = std::filesystem;

namespace amnesia {
class InboundController;
class OutboundSelector;

// =============================================================================
// BrowserEngine - Central orchestrator using all new modules
// =============================================================================

class BrowserEngine {
public:
    static BrowserEngine& GetInstance();

    bool Initialize();
    void Shutdown();
    void Navigate(const std::string& url);

    bool SetAntiFingerprintEnabled(bool enabled);
    bool SetProxyEnabled(bool enabled);
    bool SetTLSEnabled(bool enabled);

    std::string GetProfileJson() const;
    std::string GetSessionId() const;

private:
    BrowserEngine();
    ~BrowserEngine();

    BrowserEngine(const BrowserEngine&) = delete;
    BrowserEngine& operator=(const BrowserEngine&) = delete;

    bool initialized_;
    bool anti_fp_enabled_;
    bool proxy_enabled_;
    bool tls_enabled_;
    std::string current_url_;
    std::string current_session_id_;
    mutable std::mutex mutex_;
};

BrowserEngine& BrowserEngine::GetInstance() {
    static BrowserEngine instance;
    return instance;
}

BrowserEngine::BrowserEngine()
    : initialized_(false)
    , anti_fp_enabled_(true)
    , proxy_enabled_(true)
    , tls_enabled_(true)
{
}

BrowserEngine::~BrowserEngine() {
    Shutdown();
}

bool BrowserEngine::Initialize() {
    std::lock_guard<std::mutex> lock(mutex_);
    if (initialized_) return true;

    // Initialize all subsystems
    SessionManager::GetInstance();
    FingerprintProfileGenerator::GetInstance().Initialize();
    EphemeralStorage::GetInstance().Initialize("/data/data/com.amnesia.browser/ephemeral");
    CookieManager::GetInstance();
    CleanupSystem::GetInstance().Initialize();
    InjectionEngine::GetInstance().Initialize();
    JA3ConfigManager::GetInstance().Initialize();
    InboundController::GetInstance().Initialize();
    OutboundSelector::GetInstance().Initialize();

    // Create session
    current_session_id_ = SessionManager::GetInstance().CreateSession(3600);
    if (current_session_id_.empty()) return false;

    uint64_t seed = SessionManager::GetInstance().GetSessionSeed(current_session_id_);
    FingerprintProfile profile = FingerprintProfileGenerator::GetInstance().Generate(seed);
    std::string profile_json = FingerprintProfileGenerator::GetInstance().ToJson(profile);

    SessionManager::GetInstance();
    SessionManager::GetInstance();

    initialized_ = true;
    return true;
}

void BrowserEngine::Shutdown() {
    std::lock_guard<std::mutex> lock(mutex_);
    if (!initialized_) return;

    // Cleanup in reverse order
    OutboundSelector::GetInstance().Shutdown();
    InboundController::GetInstance().Shutdown();
    JA3ConfigManager::GetInstance().Shutdown();
    InjectionEngine::GetInstance().Shutdown();
    CleanupSystem::GetInstance().Shutdown();
    CookieManager::GetInstance().Shutdown();
    EphemeralStorage::GetInstance().Shutdown();
    FingerprintProfileGenerator::GetInstance().Shutdown();
    SessionManager::GetInstance();

    current_session_id_.clear();
    initialized_ = false;
}

void BrowserEngine::Navigate(const std::string& url) {
    current_url_ = url;
}

bool BrowserEngine::SetAntiFingerprintEnabled(bool enabled) {
    anti_fp_enabled_ = enabled;
    return true;
}

bool BrowserEngine::SetProxyEnabled(bool enabled) {
    proxy_enabled_ = enabled;
    return true;
}

bool BrowserEngine::SetTLSEnabled(bool enabled) {
    tls_enabled_ = enabled;
    return true;
}

std::string BrowserEngine::GetProfileJson() const {
    if (current_session_id_.empty()) return "{}";

    uint64_t seed = SessionManager::GetInstance().GetSessionSeed(current_session_id_);
    if (seed == 0) return "{}";

    FingerprintProfile profile = FingerprintProfileGenerator::GetInstance().Generate(seed);
    return FingerprintProfileGenerator::GetInstance().ToJson(profile);
}

std::string BrowserEngine::GetSessionId() const {
    return current_session_id_;
}

// =============================================================================
// JNI Exports - Single point of contact for Java
// =============================================================================

extern "C" {

JNIEXPORT jboolean JNICALL
Java_com_amnesia_browser_AmnesiaEngine_nativeInitialize(JNIEnv* env, jobject /* this */) {
    (void)env;
    return BrowserEngine::GetInstance().Initialize();
}

JNIEXPORT void JNICALL
Java_com_amnesia_browser_AmnesiaEngine_nativeShutdown(JNIEnv* env, jobject /* this */) {
    (void)env;
    BrowserEngine::GetInstance().Shutdown();
}

JNIEXPORT void JNICALL
Java_com_amnesia_browser_AmnesiaEngine_nativeNavigate(JNIEnv* env, jobject /* this */, jstring url) {
    (void)env;
    const char* chars = env->GetStringUTFChars(url, nullptr);
    if (chars) {
        BrowserEngine::GetInstance().Navigate(chars);
        env->ReleaseStringUTFChars(url, chars);
    }
}

JNIEXPORT jstring JNICALL
Java_com_amnesia_browser_AmnesiaEngine_nativeEvaluateJavaScript(JNIEnv* env, jobject /* this */, jstring script) {
    const char* chars = env->GetStringUTFChars(script, nullptr);
    std::string result;
    if (chars) {
        result = BrowserEngine::GetInstance().EvaluateJavaScript(chars);
        env->ReleaseStringUTFChars(script, chars);
    }
    return env->NewStringUTF(result.c_str());
}

// Anti-fingerprint
JNIEXPORT jstring JNICALL
Java_com_amnesia_browser_AmnesiaEngine_nativeSpoofUserAgent(JNIEnv* env, jobject /* this */) {
    (void)env;
    std::string json = BrowserEngine::GetInstance().GetProfileJson();
    size_t pos = json.find("\"user_agent\":\"");
    if (pos == std::string::npos) return env->NewStringUTF("");
    size_t start = pos + 14;
    size_t end = json.find("\"", start);
    if (end == std::string::npos) return env->NewStringUTF("");
    return env->NewStringUTF(json.substr(start, end - start).c_str());
}

JNIEXPORT jstring JNICALL
Java_com_amnesia_browser_AmnesiaEngine_nativeSpoofScreenResolution(JNIEnv* env, jobject /* this */) {
    (void)env;
    std::string json = BrowserEngine::GetInstance().GetProfileJson();
    int sw = 1080, sh = 2340;
    size_t sw_pos = json.find("\"screen_width\":");
    size_t sh_pos = json.find("\"screen_height\":");
    if (sw_pos != std::string::npos) sw = std::stoi(json.substr(sw_pos + 15));
    if (sh_pos != std::string::npos) sh = std::stoi(json.substr(sh_pos + 16));
    std::stringstream ss;
    ss << sw << "x" << sh;
    return env->NewStringUTF(ss.str().c_str());
}

JNIEXPORT void JNICALL
Java_com_amnesia_browser_AmnesiaEngine_nativeApplySpoofing(JNIEnv* env, jobject /* this */) {
    (void)env;
}

JNIEXPORT jstring JNICALL
Java_com_amnesia_browser_AmnesiaEngine_nativeSpoofLanguage(JNIEnv* env, jobject /* this */) {
    (void)env;
    std::string json = BrowserEngine::GetInstance().GetProfileJson();
    size_t pos = json.find("\"locale\":\"");
    if (pos == std::string::npos) return env->NewStringUTF("en-US");
    size_t start = pos + 10;
    size_t end = json.find("\"", start);
    if (end == std::string::npos) return env->NewStringUTF("en-US");
    return env->NewStringUTF(json.substr(start, end - start).c_str());
}

JNIEXPORT jstring JNICALL
Java_com_amnesia_browser_AmnesiaEngine_nativeSpoofTimezone(JNIEnv* env, jobject /* this */) {
    (void)env;
    std::string json = BrowserEngine::GetInstance().GetProfileJson();
    size_t pos = json.find("\"timezone_name\":\"");
    if (pos == std::string::npos) return env->NewStringUTF("UTC");
    size_t start = pos + 17;
    size_t end = json.find("\"", start);
    if (end == std::string::npos) return env->NewStringUTF("UTC");
    return env->NewStringUTF(json.substr(start, end - start).c_str());
}

// TLS
JNIEXPORT jstring JNICALL
Java_com_amnesia_browser_AmnesiaEngine_nativeGetJA3(JNIEnv* env, jobject /* this */) {
    (void)env;
    return env->NewStringUTF("769,47-53-5-10-22-13-14-25-26-15-65281-35-18-27-43,0-23-25-29131-10-11-16-5-34-51-45");
}

JNIEXPORT jstring JNICALL
Java_com_amnesia_browser_AmnesiaEngine_nativeGetJA4(JNIEnv* env, jobject /* this */) {
    (void)env;
    return env->NewStringUTF("4,1,2,03,032,01,01,01");
}

// VLESS
JNIEXPORT jboolean JNICALL
Java_com_amnesia_browser_AmnesiaEngine_nativeAddVlessLink(JNIEnv* env, jobject /* this */, jstring link) {
    (void)env;
    return JNI_TRUE;
}

JNIEXPORT jstring JNICALL
Java_com_amnesia_browser_AmnesiaEngine_nativeGetActiveLink(JNIEnv* env, jobject /* this */) {
    (void)env;
    return env->NewStringUTF("");
}

JNIEXPORT jboolean JNICALL
Java_com_amnesia_browser_AmnesiaEngine_nativeRotateLink(JNIEnv* env, jobject /* this */) {
    (void)env;
    return JNI_TRUE;
}

// Session
JNIEXPORT jstring JNICALL
Java_com_amnesia_browser_AmnesiaEngine_nativeCreateSession(JNIEnv* env, jobject /* this */, jint timeout) {
    std::string sid = SessionManager::GetInstance().CreateSession(timeout);
    return env->NewStringUTF(sid.c_str());
}

JNIEXPORT jboolean JNICALL
Java_com_amnesia_browser_AmnesiaEngine_nativeDestroySession(JNIEnv* env, jobject /* this */, jstring session_id) {
    const char* chars = env->GetStringUTFChars(session_id, nullptr);
    bool result = false;
    if (chars) {
        result = SessionManager::GetInstance().DestroySession(chars);
        env->ReleaseStringUTFChars(session_id, chars);
    }
    return result;
}

JNIEXPORT jlong JNICALL
Java_com_amnesia_browser_AmnesiaEngine_nativeGetSessionSeed(JNIEnv* env, jobject /* this */, jstring session_id) {
    const char* chars = env->GetStringUTFChars(session_id, nullptr);
    uint64_t seed = 0;
    if (chars) {
        seed = SessionManager::GetInstance().GetSessionSeed(chars);
        env->ReleaseStringUTFChars(session_id, chars);
    }
    return static_cast<jlong>(seed);
}

// Cleanup
JNIEXPORT jboolean JNICALL
Java_com_amnesia_browser_AmnesiaEngine_nativeCleanupSession(JNIEnv* env, jobject /* this */, jstring session_id) {
    (void)env;
    return CleanupSystem::GetInstance().ClearSessionData(
        session_id ? env->GetStringUTFChars(session_id, nullptr) : "");
}

JNIEXPORT jboolean JNICALL
Java_com_amnesia_browser_AmnesiaEngine_nativeClearCache(JNIEnv* env, jobject /* this */) {
    (void)env;
    return CleanupSystem::GetInstance().ClearAllCache();
}

// Profile
JNIEXPORT jstring JNICALL
Java_com_amnesia_browser_AmnesiaEngine_nativeGetProfileJson(JNIEnv* env, jobject /* this */) {
    (void)env;
    std::string json = BrowserEngine::GetInstance().GetProfileJson();
    return env->NewStringUTF(json.c_str());
}

JNIEXPORT jstring JNICALL
Java_com_amnesia_browser_AmnesiaEngine_nativeGetSessionId(JNIEnv* env, jobject /* this */) {
    (void)env;
    std::string sid = BrowserEngine::GetInstance().GetSessionId();
    return env->NewStringUTF(sid.c_str());
}

}

}  // namespace amnesia

// =============================================================================
// Crash-safe signal handlers
// =============================================================================

extern "C" {

void amnesia_signal_handler(int sig) {
    amnesia::CleanupSystem::GetInstance().ClearAllCache();
    signal(sig, SIG_DFL);
    raise(sig);
}

__attribute__((constructor))
static void amnesia_install_signal_handlers() {
    signal(SIGSEGV, amnesia_signal_handler);
    signal(SIGABRT, amnesia_signal_handler);
    signal(SIGBUS, amnesia_signal_handler);
}

}  // extern "C"
