#include "PatternScanner.h"
#include "Logger.h"
#include <cctype>

namespace hooklib {

    bool PatternScanner::parsePattern(const std::string &pattern,
                                      std::vector <std::optional<uint8_t>> &out) const {
        out.clear();
        for (size_t i = 0; i < pattern.size();) {
            while (i < pattern.size() && std::isspace((unsigned char) pattern[i])) ++i;
            if (i >= pattern.size()) break;
            if (pattern[i] == '?') {
                // Consume one or two ? tokens
                size_t j = i;
                while (j < pattern.size() && pattern[j] == '?') ++j;
                out.emplace_back(std::nullopt);
                i = j;
                continue;
            }
            if (i + 1 >= pattern.size()) return false;
            auto hexVal = [](char c) -> int {
                if (c >= '0' && c <= '9') return c - '0';
                c = (char) std::toupper((unsigned char) c);
                if (c >= 'A' && c <= 'F') return c - 'A' + 10;
                return -1;
            };
            int hi = hexVal(pattern[i]);
            int lo = hexVal(pattern[i + 1]);
            if (hi < 0 || lo < 0) return false;
            out.emplace_back(static_cast<uint8_t>((hi << 4) | lo));
            i += 2;
        }
        return true;
    }

    PatternResult PatternScanner::find(const PatternSpec &spec) const {
        auto startTime = std::chrono::steady_clock::now();
        std::vector <std::optional<uint8_t>> tokenized;
        if (!parsePattern(spec.pattern, tokenized)) {
            return {PatternStatus::ErrorPatternFormat, 0, "Bad pattern syntax", {}};
        }
        if (tokenized.empty())
            return {PatternStatus::NotFound, 0, "Empty pattern", {}};

        uint8_t *lastScan = endAddr - tokenized.size();
        for (uint8_t *cursor = baseAddr; cursor <= lastScan; ++cursor) {
            bool match = true;
            for (size_t k = 0; k < tokenized.size(); ++k) {
                if (tokenized[k].has_value() && cursor[k] != *tokenized[k]) {
                    match = false;
                    break;
                }
            }
            if (!match) continue;

            LOGD("PatternScanner: pattern %s matched at %p", spec.name.c_str(), cursor);
            uintptr_t rel = (uintptr_t)(cursor - baseAddr);
            auto elapsed = std::chrono::duration_cast<std::chrono::microseconds>(
                    std::chrono::steady_clock::now() - startTime);

            return {PatternStatus::Found, rel, "Match", elapsed};
        }
        auto elapsed = std::chrono::duration_cast<std::chrono::microseconds>(
                std::chrono::steady_clock::now() - startTime);
        return {PatternStatus::NotFound, 0, "No match", elapsed};
    }

} // namespace hooklib