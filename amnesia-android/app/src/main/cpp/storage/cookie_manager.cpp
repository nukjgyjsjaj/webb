#include "cookie_manager.h"
#include <algorithm>
#include <chrono>

namespace amnesia {

CookieManager& CookieManager::GetInstance() {
    static CookieManager instance;
    return instance;
}

CookieManager::CookieManager() : initialized_(false) {}

CookieManager::~CookieManager() {
    Shutdown();
}

bool CookieManager::Initialize(const std::string& session_id) {
    std::lock_guard<std::mutex> lock(mutex_);
    if (initialized_) return true;
    current_session_id_ = session_id;
    cookies_.clear();
    initialized_ = true;
    return true;
}

void CookieManager::Shutdown() {
    std::lock_guard<std::mutex> lock(mutex_);
    if (!initialized_) return;
    ClearSessionCookies();
    initialized_ = false;
}

bool CookieManager::SetCookie(const Cookie& cookie) {
    std::lock_guard<std::mutex> lock(mutex_);
    cookies_.push_back(cookie);
    return true;
}

bool CookieManager::DeleteCookie(const std::string& domain, const std::string& name, const std::string& path) {
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = std::remove_if(cookies_.begin(), cookies_.end(),
        [&](const Cookie& c) { return c.domain == domain && c.name == name && c.path == path; });
    if (it != cookies_.end()) {
        cookies_.erase(it, cookies_.end());
        return true;
    }
    return false;
}

std::vector<Cookie> CookieManager::FilterExpired(const std::vector<Cookie>& cookies) const {
    std::vector<Cookie> valid;
    auto now = std::chrono::system_clock::now().time_since_epoch();
    int64_t now_ms = static_cast<int64_t>(now.count() / 1000000);

    for (const auto& cookie : cookies) {
        if (cookie.expires_ms == 0 || cookie.expires_ms > now_ms) {
            valid.push_back(cookie);
        }
    }
    return valid;
}

std::vector<Cookie> CookieManager::GetCookies(const std::string& domain) const {
    std::lock_guard<std::mutex> lock(mutex_);
    std::vector<Cookie> result;
    for (const auto& cookie : cookies_) {
        if (cookie.domain == domain) {
            result.push_back(cookie);
        }
    }
    return FilterExpired(result);
}

std::vector<Cookie> CookieManager::GetAllCookies() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return FilterExpired(cookies_);
}

bool CookieManager::HasCookie(const std::string& domain, const std::string& name) const {
    std::lock_guard<std::mutex> lock(mutex_);
    for (const auto& cookie : cookies_) {
        if (cookie.domain == domain && cookie.name == name) {
            return true;
        }
    }
    return false;
}

bool CookieManager::ClearSessionCookies() {
    std::lock_guard<std::mutex> lock(mutex_);
    cookies_.clear();
    return true;
}

bool CookieManager::ClearAllCookies() {
    return ClearSessionCookies();
}

std::string CookieManager::ToHeaderString(const std::string& domain) const {
    std::stringstream ss;
    auto cookies = GetCookies(domain);

    for (size_t i = 0; i < cookies.size(); ++i) {
        if (i > 0) ss << "; ";
        ss << cookies[i].name << "=" << cookies[i].value;
    }
    return ss.str();
}

}  // namespace amnesia
