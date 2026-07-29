#include <jni.h>
#include <string>
#include <mutex>

namespace amnesia {

class TLSSpoofEngine {
public:
    static TLSSpoofEngine& GetInstance();

    bool Initialize();
    void Shutdown();

    std::string GetJA3Fingerprint();
    std::string GetJA4Fingerprint();
    std::string SpoofTLSCipherSuites();

private:
    TLSSpoofEngine();
    ~TLSSpoofEngine();

    TLSSpoofEngine(const TLSSpoofEngine&) = delete;
    TLSSpoofEngine& operator=(const TLSSpoofEngine&) = delete;

    bool initialized_;
    mutable std::mutex mutex_;
};

TLSSpoofEngine& TLSSpoofEngine::GetInstance() {
    static TLSSpoofEngine instance;
    return instance;
}

TLSSpoofEngine::TLSSpoofEngine()
    : initialized_(false)
{
}

TLSSpoofEngine::~TLSSpoofEngine() {
    Shutdown();
}

bool TLSSpoofEngine::Initialize() {
    std::lock_guard<std::mutex> lock(mutex_);
    initialized_ = true;
    return true;
}

void TLSSpoofEngine::Shutdown() {
    std::lock_guard<std::mutex> lock(mutex_);
    initialized_ = false;
}

std::string TLSSpoofEngine::GetJA3Fingerprint() {
    return "769,47-53-5-10-22-13-14-25-26-15-65281-35-18-27-43,0-23-25-29131-10-11-16-5-34-51-45";
}

std::string TLSSpoofEngine::GetJA4Fingerprint() {
    return "4,1,2,03,032,01,01,01";
}

std::string TLSSpoofEngine::SpoofTLSCipherSuites() {
    return "TLS_ECDHE_ECDSA_WITH_AES_256_GCM_SHA384,TLS_ECDHE_RSA_WITH_AES_256_GCM_SHA384";
}

}  // namespace amnesia
