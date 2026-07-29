#ifndef AMNESIA_STORAGE_EPHEMERAL_STORAGE_H
#define AMNESIA_STORAGE_EPHEMERAL_STORAGE_H

#include <string>
#include <vector>
#include <cstdint>
#include <mutex>

namespace amnesia {

class EphemeralStorage {
public:
    static EphemeralStorage& GetInstance();

    bool Initialize(const std::string& base_path);
    void Shutdown();

    bool WriteSessionData(const std::string& session_id, const std::string& key, const std::string& value);
    std::string ReadSessionData(const std::string& session_id, const std::string& key);
    bool DeleteSessionData(const std::string& session_id, const std::string& key);
    bool ClearSession(const std::string& session_id);
    bool ClearAllSessions();

    std::vector<std::string> ListSessionKeys(const std::string& session_id) const;

    size_t GetSessionDataSize(const std::string& session_id) const;

private:
    EphemeralStorage();
    ~EphemeralStorage();

    EphemeralStorage(const EphemeralStorage&) = delete;
    EphemeralStorage& operator=(const EphemeralStorage&) = delete;

    std::string GetSessionPath(const std::string& session_id) const;
    bool SecureDeleteFile(const std::string& path) const;
    bool SecureWipeBuffer(uint8_t* data, size_t size) const;

    std::string base_path_;
    mutable std::mutex mutex_;
    bool initialized_;
};

}  // namespace amnesia

#endif  // AMNESIA_STORAGE_EPHEMERAL_STORAGE_H
