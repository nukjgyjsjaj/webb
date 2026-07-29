#ifndef AMNESIA_CORE_AMNESIA_INIT_H
#define AMNESIA_CORE_AMNESIA_INIT_H

#include <string>
#include <mutex>

namespace amnesia {

class AmnesiaInit {
public:
    static AmnesiaInit& GetInstance();

    bool Initialize(const std::string& app_data_path);
    void Shutdown();

    std::string GetCurrentSessionId() const;
    uint64_t GetCurrentSessionSeed() const;
    std::string GetProfileJson() const;
    bool IsInitialized() const;

    bool CreateNewSession();
    bool DestroyCurrentSession();

private:
    AmnesiaInit();
    ~AmnesiaInit();

    AmnesiaInit(const AmnesiaInit&) = delete;
    AmnesiaInit& operator=(const AmnesiaInit&) = delete;

    bool initialized_;
    mutable std::mutex mutex_;
    std::string app_data_path_;
};

}  // namespace amnesia

#endif  // AMNESIA_CORE_AMNESIA_INIT_H
