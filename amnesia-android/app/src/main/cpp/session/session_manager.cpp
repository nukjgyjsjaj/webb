#include <jni.h>
#include <string>
#include <vector>
#include <cstdio>
#include <mutex>
#include <chrono>
#include <random>
#include <algorithm>

namespace amnesia {

class SessionManager {
public:
    static SessionManager& GetInstance();

    std::string CreateSession(int timeout_seconds = 3600);
    bool DestroySession(const std::string& session_id);
    bool IsSessionValid(const std::string& session_id) const;
    uint64_t GetSessionSeed(const std::string& session_id) const;

    void ExtendSession(const std::string& session_id, int additional_seconds);
    size_t GetActiveSessionCount() const;
    std::vector<std::string> GetActiveSessionIds() const;

private:
    SessionManager();
    ~SessionManager();

    SessionManager(const SessionManager&) = delete;
    SessionManager& operator=(const SessionManager&) = delete;

    struct Session {
        std::string id;
        uint64_t seed;
        std::chrono::steady_clock::time_point created_at;
        std::chrono::steady_clock::time_point last_accessed;
        int timeout_seconds;
        bool active;
    };

    Session* FindSession(const std::string& session_id);
    const Session* FindSession(const std::string& session_id) const;
    std::string GenerateSessionId(uint64_t seed);
    uint64_t GenerateSeed();

    std::mutex mutex_;
    std::vector<Session> sessions_;
    bool initialized_;
};

SessionManager& SessionManager::GetInstance() {
    static SessionManager instance;
    return instance;
}

SessionManager::SessionManager()
    : initialized_(false)
{
    std::random_device rd;
    std::mt19937_64 gen(rd());
    std::uniform_int_distribution<uint64_t> dist;
}

SessionManager::~SessionManager() {
    Shutdown();
}

std::string SessionManager::CreateSession(int timeout_seconds) {
    std::lock_guard<std::mutex> lock(mutex_);
    if (!initialized_) return "";

    Session session;
    session.seed = GenerateSeed();
    session.id = GenerateSessionId(session.seed);
    session.created_at = std::chrono::steady_clock::now();
    session.last_accessed = session.created_at;
    session.timeout_seconds = timeout_seconds > 0 ? timeout_seconds : 3600;
    session.active = true;

    sessions_.push_back(session);
    return session.id;
}

bool SessionManager::DestroySession(const std::string& session_id) {
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = std::find_if(sessions_.begin(), sessions_.end(),
        [&session_id](const Session& s) { return s.id == session_id; });
    if (it != sessions_.end()) {
        it->active = false;
        sessions_.erase(it);
        return true;
    }
    return false;
}

bool SessionManager::IsSessionValid(const std::string& session_id) const {
    std::lock_guard<std::mutex> lock(mutex_);
    const Session* session = FindSession(session_id);
    if (!session || !session->active) return false;

    auto now = std::chrono::steady_clock::now();
    auto age = std::chrono::duration_cast<std::chrono::seconds>(now - session->last_accessed).count();
    return age < session->timeout_seconds;
}

uint64_t SessionManager::GetSessionSeed(const std::string& session_id) const {
    std::lock_guard<std::mutex> lock(mutex_);
    const Session* session = FindSession(session_id);
    if (session) {
        return session->seed;
    }
    return 0;
}

void SessionManager::ExtendSession(const std::string& session_id, int additional_seconds) {
    std::lock_guard<std::mutex> lock(mutex_);
    Session* session = FindSession(session_id);
    if (session) {
        session->timeout_seconds += additional_seconds;
        session->last_accessed = std::chrono::steady_clock::now();
    }
}

size_t SessionManager::GetActiveSessionCount() const {
    std::lock_guard<std::mutex> lock(mutex_);
    size_t count = 0;
    auto now = std::chrono::steady_clock::now();
    for (const auto& session : sessions_) {
        if (!session.active) continue;
        auto age = std::chrono::duration_cast<std::chrono::seconds>(now - session.last_accessed).count();
        if (age < session.timeout_seconds) {
            count++;
        }
    }
    return count;
}

std::vector<std::string> SessionManager::GetActiveSessionIds() const {
    std::lock_guard<std::mutex> lock(mutex_);
    std::vector<std::string> ids;
    auto now = std::chrono::steady_clock::now();
    for (const auto& session : sessions_) {
        if (!session.active) continue;
        auto age = std::chrono::duration_cast<std::chrono::seconds>(now - session.last_accessed).count();
        if (age < session.timeout_seconds) {
            ids.push_back(session.id);
        }
    }
    return ids;
}

SessionManager::Session* SessionManager::FindSession(const std::string& session_id) {
    for (auto& session : sessions_) {
        if (session.id == session_id) {
            session.last_accessed = std::chrono::steady_clock::now();
            return &session;
        }
    }
    return nullptr;
}

const SessionManager::Session* SessionManager::FindSession(const std::string& session_id) const {
    for (const auto& session : sessions_) {
        if (session.id == session_id) {
            return &session;
        }
    }
    return nullptr;
}

std::string SessionManager::GenerateSessionId(uint64_t seed) {
    std::mt19937_64 gen(seed);
    std::uniform_int_distribution<uint64_t> dist;
    uint64_t val = dist(gen);
    char buf[17];
    snprintf(buf, sizeof(buf), "%016llx", (unsigned long long)val);
    return std::string(buf);
}

uint64_t SessionManager::GenerateSeed() {
    std::random_device rd;
    std::mt19937_64 gen(rd());
    std::uniform_int_distribution<uint64_t> dist;
    return dist(gen);
}

extern "C" {

JNIEXPORT jstring JNICALL
Java_com_amnesia_browser_AmnesiaEngine_nativeCreateSession(JNIEnv* env, jobject /* this */, jint timeout) {
    std::string session_id = SessionManager::GetInstance().CreateSession(timeout);
    return env->NewStringUTF(session_id.c_str());
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
    return (jlong)seed;
}

}

}
