#ifndef AMNESIA_STORAGE_COOKIE_MANAGER_H
#define AMNESIA_STORAGE_COOKIE_MANAGER_H

#include <string>
#include <vector>
#include <map>
#include <cstdint>
#include <mutex>

namespace amnesia {

struct Cookie {
    std::string name;
    std::string value;
    std::string domain;
    std::string path;
    bool secure;
    bool http_only;
    bool host_only;
    int64_t expires_ms;
    std::string same_site;
};

class CookieManager {
public:
    static CookieManager& GetInstance();

    bool Initialize(const std::string& session_id);
    void Shutdown();

    bool SetCookie(const Cookie& cookie);
    bool DeleteCookie(const std::string& domain, const std::string& name, const std::string& path);
    std::vector<Cookie> GetCookies(const std::string& domain) const;
    std::vector<Cookie> GetAllCookies() const;
    bool HasCookie(const std::string& domain, const std::string& name) const;

    bool ClearSessionCookies();
    bool ClearAllCookies();

    std::string ToHeaderString(const std::string& domain) const;

private:
    CookieManager();
    ~CookieManager();

    CookieManager(const CookieManager&) = delete;
    CookieManager& operator=(const CookieManager&) = delete;

    std::vector<Cookie> FilterExpired(const std::vector<Cookie>& cookies) const;

    std::string current_session_id_;
    std::vector<Cookie> cookies_;
    mutable std::mutex mutex_;
    bool initialized_;
};

}  // namespace amnesia

#endif  // AMNESIA_STORAGE_COOKIE_MANAGER_H
