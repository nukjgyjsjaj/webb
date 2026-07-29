#include "amnesia_init.h"
#include "session/session_manager.h"
#include "storage/ephemeral_storage.h"
#include "storage/cookie_manager.h"
#include "cleanup/cleanup_system.h"
#include "anti_fp/fingerprint_profile.h"
#include "js_injection/injection_engine.h"
#include <string>
#include <mutex>
#include <chrono>

namespace amnesia {

AmnesiaInit& AmnesiaInit::GetInstance() {
    static AmnesiaInit instance;
    return instance;
}

AmnesiaInit::AmnesiaInit() : initialized_(false) {}

AmnesiaInit::~AmnesiaInit() {
    Shutdown();
}

bool AmnesiaInit::Initialize(const std::string& app_data_path) {
    std::lock_guard<std::mutex> lock(mutex_);
    if (initialized_) return true;

    app_data_path_ = app_data_path;

    // Initialize all subsystems in dependency order
    SessionManager::GetInstance();
    EphemeralStorage::GetInstance().Initialize(app_data_path_ + "/ephemeral");
    CookieManager::GetInstance();
    CleanupSystem::GetInstance().Initialize();
    FingerprintProfileGenerator::GetInstance().Initialize();
    InjectionEngine::GetInstance().Initialize();

    // Create first session
    CreateNewSession();

    initialized_ = true;
    return true;
}

void AmnesiaInit::Shutdown() {
    std::lock_guard<std::mutex> lock(mutex_);
    if (!initialized_) return;

    // Destroy current session first
    DestroyCurrentSession();

    // Shutdown in reverse order
    InjectionEngine::GetInstance().Shutdown();
    FingerprintProfileGenerator::GetInstance().Shutdown();
    CleanupSystem::GetInstance().Shutdown();
    CookieManager::GetInstance().Shutdown();
    EphemeralStorage::GetInstance().Shutdown();
    SessionManager::GetInstance();

    initialized_ = false;
}

bool AmnesiaInit::CreateNewSession() {
    std::string session_id = SessionManager::GetInstance().CreateSession(3600);
    if (session_id.empty()) return false;

    uint64_t seed = SessionManager::GetInstance().GetSessionSeed(session_id);
    FingerprintProfile profile = FingerprintProfileGenerator::GetInstance().Generate(seed);
    std::string profile_json = FingerprintProfileGenerator::GetInstance().ToJson(profile);

    SessionManager::GetInstance();
    return true;
}

bool AmnesiaInit::DestroyCurrentSession() {
    SessionManager& sm = SessionManager::GetInstance();
    auto ids = sm.GetActiveSessionIds();
    for (const auto& sid : ids) {
        sm.DestroySession(sid);
    }
    return true;
}

std::string AmnesiaInit::GetCurrentSessionId() const {
    auto ids = SessionManager::GetInstance().GetActiveSessionIds();
    if (ids.empty()) return "";
    return ids[0];
}

uint64_t AmnesiaInit::GetCurrentSessionSeed() const {
    std::string sid = GetCurrentSessionId();
    if (sid.empty()) return 0;
    return SessionManager::GetInstance().GetSessionSeed(sid);
}

std::string AmnesiaInit::GetProfileJson() const {
    std::string sid = GetCurrentSessionId();
    if (sid.empty()) return "{}";
    return SessionManager::GetInstance().GetSessionProfileJson(sid);
}

bool AmnesiaInit::IsInitialized() const {
    return initialized_;
}

}  // namespace amnesia
