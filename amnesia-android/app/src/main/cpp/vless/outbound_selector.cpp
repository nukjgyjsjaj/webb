#include "outbound_selector.h"
#include <algorithm>
#include <random>
#include <chrono>
#include <mutex>

namespace amnesia {

OutboundSelector& OutboundSelector::GetInstance() {
    static OutboundSelector instance;
    return instance;
}

OutboundSelector::OutboundSelector() : initialized_(false) {}

OutboundSelector::~OutboundSelector() {
    Shutdown();
}

bool OutboundSelector::Initialize() {
    std::lock_guard<std::mutex> lock(mutex_);
    if (initialized_) return true;
    initialized_ = true;
    return true;
}

void OutboundSelector::Shutdown() {
    std::lock_guard<std::mutex> lock(mutex_);
    if (!initialized_) return;
    links_.clear();
    initialized_ = false;
}

LinkScore* OutboundSelector::FindLink(const std::string& link) {
    for (auto& ls : links_) {
        if (ls.link == link) return &ls;
    }
    return nullptr;
}

bool OutboundSelector::AddLink(const std::string& link) {
    std::lock_guard<std::mutex> lock(mutex_);
    if (FindLink(link)) return false;

    LinkScore score;
    score.link = link;
    score.latency_ms = 0;
    score.failures = 0;
    score.active = true;
    score.last_used = 0;
    links_.push_back(score);
    return true;
}

bool OutboundSelector::RemoveLink(const std::string& link) {
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = std::remove_if(links_.begin(), links_.end(),
        [&](const LinkScore& ls) { return ls.link == link; });
    if (it != links_.end()) {
        links_.erase(it, links_.end());
        return true;
    }
    return false;
}

std::string OutboundSelector::SelectLinkByStrategy(bool random) {
    if (links_.empty()) return "";

    std::vector<LinkScore*> active;
    for (auto& ls : links_) {
        if (ls.active) active.push_back(&ls);
    }

    if (active.empty()) return "";

    if (random) {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<size_t> dist(0, active.size() - 1);
        return active[dist(gen)]->link;
    }

    // Best latency strategy
    LinkScore* best = active[0];
    for (auto* ls : active) {
        if (ls->latency_ms == 0 || (best->latency_ms > 0 && ls->latency_ms < best->latency_ms)) {
            best = ls;
        }
    }
    return best->link;
}

std::string OutboundSelector::SelectRandomLink() {
    return SelectLinkByStrategy(true);
}

std::string OutboundSelector::SelectBestLink() {
    return SelectLinkByStrategy(false);
}

bool OutboundSelector::ReportFailure(const std::string& link) {
    std::lock_guard<std::mutex> lock(mutex_);
    LinkScore* ls = FindLink(link);
    if (!ls) return false;
    ls->failures++;
    if (ls->failures >= 3) ls->active = false;
    return true;
}

bool OutboundSelector::ReportSuccess(const std::string& link, int latency_ms) {
    std::lock_guard<std::mutex> lock(mutex_);
    LinkScore* ls = FindLink(link);
    if (!ls) return false;
    ls->latency_ms = latency_ms > 0 ? latency_ms : ls->latency_ms;
    ls->active = true;
    ls->last_used = static_cast<uint64_t>(std::chrono::steady_clock::now().time_since_epoch().count());
    return true;
}

size_t OutboundSelector::GetActiveCount() const {
    std::lock_guard<std::mutex> lock(mutex_);
    size_t count = 0;
    for (const auto& ls : links_) {
        if (ls.active) count++;
    }
    return count;
}

std::vector<std::string> OutboundSelector::GetAllLinks() const {
    std::lock_guard<std::mutex> lock(mutex_);
    std::vector<std::string> result;
    for (const auto& ls : links_) {
        if (ls.active) result.push_back(ls.link);
    }
    return result;
}

}  // namespace amnesia
