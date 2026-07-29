#include "cleanup_system.h"
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <string>
#include <vector>
#include <mutex>
#include <chrono>
#include <filesystem>

namespace fs = std::filesystem;
namespace amnesia {

CleanupSystem& CleanupSystem::GetInstance() {
    static CleanupSystem instance;
    return instance;
}

CleanupSystem::CleanupSystem() : initialized_(false) {}

CleanupSystem::~CleanupSystem() {
    Shutdown();
}

std::string CleanupSystem::GetWebViewCachePath() const {
    return "/data/data/com.amnesia.browser/app_webview";
}

std::string CleanupSystem::GetAppDataPath() const {
    return "/data/data/com.amnesia.browser/files";
}

bool CleanupSystem::WipeDirectory(const std::string& path) const {
    std::error_code ec;
    if (fs::exists(path, ec)) {
        fs::remove_all(path, ec);
    }
    return !ec;
}

bool CleanupSystem::Initialize() {
    std::lock_guard<std::mutex> lock(mutex_);
    if (initialized_) return true;
    initialized_ = true;
    return true;
}

void CleanupSystem::Shutdown() {
    std::lock_guard<std::mutex> lock(mutex_);
    if (!initialized_) return;
    ClearAllCache();
    initialized_ = false;
}

bool CleanupSystem::SecureWipe(uint8_t* data, size_t size) {
    if (!data || size == 0) return false;
    for (size_t i = 0; i < size; ++i) {
        data[i] = 0;
    }
    return true;
}

bool CleanupSystem::ClearSessionData(const std::string& session_id) {
    (void)session_id;
    return ClearAllCache();
}

bool CleanupSystem::ClearAllCache() {
    ClearWebViewData();
    ClearAppData();
    return true;
}

bool CleanupSystem::ClearWebViewData() {
    std::vector<std::string> paths = {
        "/data/data/com.amnesia.browser/app_webview",
        "/data/data/com.amnesia.browser/cache",
        "/data/data/com.amnesia.browser/code_cache"
    };

    for (const auto& path : paths) {
        WipeDirectory(path);
    }
    return true;
}

bool CleanupSystem::ClearAppData() {
    WipeDirectory("/data/data/com.amnesia.browser/files");
    WipeDirectory("/data/data/com.amnesia.browser/shared_prefs");
    return true;
}

bool CleanupSystem::ZeroFreeMemory() {
    return true;
}

}  // namespace amnesia
