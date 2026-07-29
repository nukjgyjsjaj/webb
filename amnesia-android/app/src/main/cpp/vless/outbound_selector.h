#ifndef AMNESIA_VLESS_OUTBOUND_SELECTOR_H
#define AMNESIA_VLESS_OUTBOUND_SELECTOR_H

#include <string>
#include <vector>
#include <cstdint>
#include <mutex>

namespace amnesia {

struct LinkScore {
    std::string link;
    int latency_ms;
    int failures;
    bool active;
    uint64_t last_used;
};

class OutboundSelector {
public:
    static OutboundSelector& GetInstance();

    bool Initialize();
    void Shutdown();

    bool AddLink(const std::string& link);
    bool RemoveLink(const std::string& link);
    std::string SelectBestLink();
    std::string SelectRandomLink();
    bool ReportFailure(const std::string& link);
    bool ReportSuccess(const std::string& link, int latency_ms);

    size_t GetActiveCount() const;
    std::vector<std::string> GetAllLinks() const;

private:
    OutboundSelector();
    ~OutboundSelector();

    OutboundSelector(const OutboundSelector&) = delete;
    OutboundSelector& operator=(const OutboundSelector&) = delete;

    LinkScore* FindLink(const std::string& link);
    std::string SelectLinkByStrategy(bool random);

    std::vector<LinkScore> links_;
    mutable std::mutex mutex_;
    bool initialized_;
};

}  // namespace amnesia

#endif  // AMNESIA_VLESS_OUTBOUND_SELECTOR_H
