#ifndef AMNESIA_CLEANUP_CLEANUP_SYSTEM_H
#define AMNESIA_CLEANUP_CLEANUP_SYSTEM_H

#include <string>
#include <cstdint>
#include <mutex>

namespace amnesia {

class CleanupSystem {
public:
    static CleanupSystem& GetInstance();

    bool Initialize();
    void Shutdown();

    bool ClearSessionData(const std::string& session_id);
    bool ClearAllCache();
    bool SecureWipe(uint8_t* data, size_t size);
    bool ClearWebViewData();
    bool ClearAppData();
    bool ZeroFreeMemory();

private:
    CleanupSystem();
    ~CleanupSystem();

    CleanupSystem(const CleanupSystem&) = delete;
    CleanupSystem& operator=(const CleanupSystem&) = delete;

    std::string GetWebViewCachePath() const;
    std::string GetAppDataPath() const;
    bool WipeDirectory(const std::string& path) const;

    mutable std::mutex mutex_;
    bool initialized_;
};

}  // namespace amnesia

#endif  // AMNESIA_CLEANUP_CLEANUP_SYSTEM_H
