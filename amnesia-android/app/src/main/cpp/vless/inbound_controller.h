#ifndef AMNESIA_VLESS_INBOUND_CONTROLLER_H
#define AMNESIA_VLESS_INBOUND_CONTROLLER_H

#include <string>
#include <cstdint>
#include <mutex>

namespace amnesia {

struct VlessLink {
    std::string raw;
    std::string uuid;
    std::string server;
    uint16_t port;
    std::string transport;
    std::string security;
    std::string path;
    std::string host;
    bool valid;
    int latency_ms;
};

class InboundController {
public:
    static InboundController& GetInstance();

    bool Initialize();
    void Shutdown();

    bool StartLocalSOCKS5(int port);
    bool StopLocalSOCKS5();
    bool IsRunning() const;
    int GetLocalPort() const;

    bool ConfigureXray(const std::string& vless_link);
    std::string GenerateXrayConfig(const VlessLink& link) const;

private:
    InboundController();
    ~InboundController();

    InboundController(const InboundController&) = delete;
    InboundController& operator=(const InboundController&) = delete;

    bool ParseVlessLink(const std::string& link_text, VlessLink& out_link);

    VlessLink active_link_;
    int local_port_;
    bool running_;
    mutable std::mutex mutex_;
    bool initialized_;
};

}  // namespace amnesia

#endif  // AMNESIA_VLESS_INBOUND_CONTROLLER_H
