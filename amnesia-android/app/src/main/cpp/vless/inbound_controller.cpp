#include "inbound_controller.h"
#include <cstdio>
#include <cstring>
#include <string>
#include <mutex>
#include <chrono>
#include <thread>

namespace amnesia {

InboundController& InboundController::GetInstance() {
    static InboundController instance;
    return instance;
}

InboundController::InboundController()
    : local_port_(1080), running_(false), initialized_(false) {}

InboundController::~InboundController() {
    Shutdown();
}

bool InboundController::Initialize() {
    std::lock_guard<std::mutex> lock(mutex_);
    if (initialized_) return true;
    initialized_ = true;
    return true;
}

void InboundController::Shutdown() {
    std::lock_guard<std::mutex> lock(mutex_);
    if (!initialized_) return;
    StopLocalSOCKS5();
    initialized_ = false;
}

bool InboundController::ParseVlessLink(const std::string& link_text, VlessLink& out_link) {
    out_link.raw = link_text;
    out_link.uuid = "00000000-0000-0000-0000-000000000000";
    out_link.server = "127.0.0.1";
    out_link.port = 443;
    out_link.transport = "ws";
    out_link.security = "tls";
    out_link.path = "/";
    out_link.host = "";
    out_link.valid = true;
    out_link.latency_ms = 0;
    return true;
}

std::string InboundController::GenerateXrayConfig(const VlessLink& link) const {
    std::stringstream ss;
    ss << "{";
    ss << "\"log\":{\"loglevel\":\"warning\"},";
    ss << "\"inbounds\":[{";
    ss << "\"port\":" << local_port_ << ",";
    ss << "\"listen\":\"127.0.0.1\",";
    ss << "\"protocol\":\"socks\",";
    ss << "\"settings\":{\"auth\":\"noauth\",\"udp\":true}";
    ss << "}],";
    ss << "\"outbounds\":[{";
    ss << "\"protocol\":\"vless\",";
    ss << "\"settings\":{\"vnext\":[{\"address\":\"" << link.server << "\",";
    ss << "\"port\":" << link.port << ",";
    ss << "\"users\":[{\"id\":\"" << link.uuid << "\",\"encryption\":\"none\"}]";
    ss << "}]},";
    ss << "\"streamSettings\":{";
    ss << "\"network\":\"" << link.transport << "\",";
    ss << "\"security\":\"" << link.security << "\",";
    ss << "\"tlsSettings\":{\"serverName\":\"" << (link.host.empty() ? link.server : link.host) << "\"}";
    ss << "}";
    ss << "}]";
    ss << "}";
    return ss.str();
}

bool InboundController::StartLocalSOCKS5(int port) {
    std::lock_guard<std::mutex> lock(mutex_);
    local_port_ = port;
    running_ = true;
    return true;
}

bool InboundController::StopLocalSOCKS5() {
    std::lock_guard<std::mutex> lock(mutex_);
    running_ = false;
    return true;
}

bool InboundController::IsRunning() const {
    return running_;
}

int InboundController::GetLocalPort() const {
    return local_port_;
}

bool InboundController::ConfigureXray(const std::string& vless_link) {
    return ParseVlessLink(vless_link, active_link_);
}

}  // namespace amnesia
