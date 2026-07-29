#include "ephemeral_storage.h"
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <algorithm>
#include <fstream>
#include <sstream>

namespace amnesia {

EphemeralStorage& EphemeralStorage::GetInstance() {
    static EphemeralStorage instance;
    return instance;
}

EphemeralStorage::EphemeralStorage() : initialized_(false) {}

EphemeralStorage::~EphemeralStorage() {
    Shutdown();
}

bool EphemeralStorage::Initialize(const std::string& base_path) {
    std::lock_guard<std::mutex> lock(mutex_);
    if (initialized_) return true;
    base_path_ = base_path;
    initialized_ = true;
    return true;
}

void EphemeralStorage::Shutdown() {
    std::lock_guard<std::mutex> lock(mutex_);
    if (!initialized_) return;
    ClearAllSessions();
    initialized_ = false;
}

std::string EphemeralStorage::GetSessionPath(const std::string& session_id) const {
    std::stringstream ss;
    ss << base_path_ << "/session_" << session_id;
    return ss.str();
}

bool EphemeralStorage::SecureWipeBuffer(uint8_t* data, size_t size) const {
    if (!data || size == 0) return false;
    for (size_t i = 0; i < size; ++i) {
        data[i] = 0;
    }
    return true;
}

bool EphemeralStorage::SecureDeleteFile(const std::string& path) const {
    std::ifstream file(path, std::ios::binary);
    if (!file.is_open()) return false;

    file.seekg(0, std::ios::end);
    size_t size = static_cast<size_t>(file.tellg());
    file.seekg(0, std::ios::beg);

    std::vector<uint8_t> buffer(size);
    file.read(reinterpret_cast<char*>(buffer.data()), size);
    file.close();
    SecureWipeBuffer(buffer.data(), buffer.size());

    std::remove(path.c_str());
    return true;
}

bool EphemeralStorage::WriteSessionData(const std::string& session_id, const std::string& key, const std::string& value) {
    if (!initialized_) return false;
    std::string session_path = GetSessionPath(session_id);
    std::string file_path = session_path + "/" + key;

    std::ofstream file(file_path, std::ios::binary);
    if (!file.is_open()) return false;

    file.write(value.data(), value.size());
    file.close();
    return true;
}

std::string EphemeralStorage::ReadSessionData(const std::string& session_id, const std::string& key) {
    if (!initialized_) return "";
    std::string file_path = GetSessionPath(session_id) + "/" + key;

    std::ifstream file(file_path, std::ios::binary);
    if (!file.is_open()) return "";

    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

bool EphemeralStorage::DeleteSessionData(const std::string& session_id, const std::string& key) {
    if (!initialized_) return false;
    std::string file_path = GetSessionPath(session_id) + "/" + key;
    SecureDeleteFile(file_path);
    return true;
}

bool EphemeralStorage::ClearSession(const std::string& session_id) {
    if (!initialized_) return false;
    std::string session_path = GetSessionPath(session_id);
    std::error_code ec;
    std::filesystem::remove_all(session_path, ec);
    return !ec;
}

bool EphemeralStorage::ClearAllSessions() {
    if (!initialized_) return false;
    std::error_code ec;
    for (const auto& entry : std::filesystem::directory_iterator(base_path_)) {
        if (entry.is_directory()) {
            std::filesystem::remove_all(entry.path(), ec);
        }
    }
    return !ec;
}

std::vector<std::string> EphemeralStorage::ListSessionKeys(const std::string& session_id) const {
    std::vector<std::string> keys;
    std::string session_path = GetSessionPath(session_id);

    std::error_code ec;
    for (const auto& entry : std::filesystem::directory_iterator(session_path, ec)) {
        if (entry.is_regular_file()) {
            keys.push_back(entry.path().filename().string());
        }
    }
    return keys;
}

size_t EphemeralStorage::GetSessionDataSize(const std::string& session_id) const {
    size_t total_size = 0;
    std::string session_path = GetSessionPath(session_id);

    std::error_code ec;
    for (const auto& entry : std::filesystem::directory_iterator(session_path, ec)) {
        if (entry.is_regular_file()) {
            total_size += static_cast<size_t>(entry.file_size());
        }
    }
    return total_size;
}

}  // namespace amnesia
