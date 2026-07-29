#include <jni.h>
#include <string>
#include <vector>
#include <algorithm>
#include <mutex>
#include <chrono>
#include <random>

namespace amnesia {

struct VlessLink {
    std::string uuid;
    std::string server;
    uint16_t port;
    std::string transport;
    std::string security;
    std::string raw_link;
    bool valid;
    int latency_ms;
    std::chrono::steady_clock::time_point last_tested;
};

class VlessProxyEngine {
public:
    static VlessProxyEngine& GetInstance();

    bool Initialize();
    void Shutdown();

    bool AddVlessLink(const std::string& link);
    bool RemoveVlessLink(const std::string& link);
    std::string GetActiveLink();
    bool RotateLink();
    bool TestLink(const std::string& link, int timeout);

    void SetAutoRotate(bool enable) { auto_rotate_ = enable; }
    bool IsAutoRotateEnabled() const { return auto_rotate_; }

private:
    VlessProxyEngine();
    ~VlessProxyEngine();

    VlessProxyEngine(const VlessProxyEngine&) = delete;
    VlessProxyEngine& operator=(const VlessProxyEngine&) = delete;

    bool ParseVlessLink(const std::string& link_text, VlessLink& out_link);
    bool ValidateLink(const VlessLink& link);
    size_t SelectRandomValidLink();

    std::vector<VlessLink> links_;
    std::vector<VlessLink> valid_links_;
    std::string current_link_;
    bool initialized_;
    bool auto_rotate_;
    mutable std::mutex mutex_;
};

VlessProxyEngine& VlessProxyEngine::GetInstance() {
    static VlessProxyEngine instance;
    return instance;
}

VlessProxyEngine::VlessProxyEngine()
    : initialized_(false)
    , auto_rotate_(true)
{
}

VlessProxyEngine::~VlessProxyEngine() {
    Shutdown();
}

bool VlessProxyEngine::Initialize() {
    std::lock_guard<std::mutex> lock(mutex_);
    if (initialized_) return true;
    initialized_ = true;
    return true;
}

void VlessProxyEngine::Shutdown() {
    std::lock_guard<std::mutex> lock(mutex_);
    links_.clear();
    valid_links_.clear();
    current_link_.clear();
    initialized_ = false;
}

bool VlessProxyEngine::AddVlessLink(const std::string& link) {
    std::lock_guard<std::mutex> lock(mutex_);
    VlessLink vlink;
    if (!ParseVlessLink(link, vlink)) return false;
    vlink.valid = true;
    links_.push_back(vlink);
    valid_links_.push_back(vlink);
    return true;
}

bool VlessProxyEngine::RemoveVlessLink(const std::string& link) {
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = std::remove_if(links_.begin(), links_.end(),
        [&link](const VlessLink& l) { return l.raw_link == link; });
    if (it != links_.end()) {
        links_.erase(it, links_.end());
    }
    it = std::remove_if(valid_links_.begin(), valid_links_.end(),
        [&link](const VlessLink& l) { return l.raw_link == link; });
    if (it != valid_links_.end()) {
        valid_links_.erase(it, valid_links_.end());
    }
    return true;
}

std::string VlessProxyEngine::GetActiveLink() {
    std::lock_guard<std::mutex> lock(mutex_);
    if (!current_link_.empty() && !valid_links_.empty()) {
        return current_link_;
    }
    RotateLink();
    return current_link_;
}

bool VlessProxyEngine::RotateLink() {
    std::lock_guard<std::mutex> lock(mutex_);
    size_t index = SelectRandomValidLink();
    if (index < valid_links_.size()) {
        current_link_ = valid_links_[index].raw_link;
        return true;
    }
    return false;
}

bool VlessProxyEngine::TestLink(const std::string& link, int timeout) {
    std::lock_guard<std::mutex> lock(mutex_);
    for (auto& vlink : links_) {
        if (vlink.raw_link == link) {
            vlink.last_tested = std::chrono::steady_clock::now();
            vlink.latency_ms = timeout > 0 ? timeout : 100;
            return true;
        }
    }
    return false;
}

size_t VlessProxyEngine::SelectRandomValidLink() {
    if (valid_links_.empty()) return 0;
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<size_t> dist(0, valid_links_.size() - 1);
    return dist(gen);
}

bool VlessProxyEngine::ParseVlessLink(const std::string& link_text, VlessLink& out_link) {
    if (link_text.size() < 8 || link_text.substr(0, 7) != "vless://") return false;
    out_link.raw_link = link_text;
    out_link.valid = true;
    return true;
}

bool VlessProxyEngine::ValidateLink(const VlessLink& link) {
    if (link.raw_link.empty()) return false;
    if (link.raw_link.size() < 8) return false;
    return true;
}

extern "C" {

JNIEXPORT jboolean JNICALL
Java_com_amnesia_browser_AmnesiaEngine_nativeAddVlessLink(JNIEnv* env, jobject /* this */, jstring link) {
    const char* chars = env->GetStringUTFChars(link, nullptr);
    bool result = false;
    if (chars) {
        result = VlessProxyEngine::GetInstance().AddVlessLink(chars);
        env->ReleaseStringUTFChars(link, chars);
    }
    return result;
}

JNIEXPORT jstring JNICALL
Java_com_amnesia_browser_AmnesiaEngine_nativeGetActiveLink(JNIEnv* env, jobject /* this */) {
    std::string active = VlessProxyEngine::GetInstance().GetActiveLink();
    return env->NewStringUTF(active.c_str());
}

JNIEXPORT jboolean JNICALL
Java_com_amnesia_browser_AmnesiaEngine_nativeRotateLink(JNIEnv* env, jobject /* this */) {
    (void)env;
    return VlessProxyEngine::GetInstance().RotateLink();
}

}

}
