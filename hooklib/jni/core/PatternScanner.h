#pragma once

#include <cstdint>
#include <string>
#include <vector>
#include <optional>
#include <chrono>

namespace hooklib {

    struct PatternSpec {
        std::string name;
        std::string pattern;          // e.g. "FE 0F 1D F8 ?? ?? ?? ??"
        bool relative = false;        // If true, compute relative offset semantics
        size_t verifyBytes = 0;       // optional: bytes to sanity-check after match
    };

    enum class PatternStatus {
        Found,
        NotFound,
        ErrorPatternFormat
    };

    struct PatternResult {
        PatternStatus status;
        uintptr_t relativeOffset = 0;
        std::string message;
        std::chrono::microseconds duration{};
    };

    class PatternScanner {
    public:
        PatternScanner(uint8_t *base, uint8_t *end) : baseAddr(base), endAddr(end) {}

        PatternResult find(const PatternSpec &spec) const;

    private:
        bool parsePattern(const std::string &pattern,
                          std::vector<std::optional<uint8_t>> &out) const;

        uint8_t *baseAddr;
        uint8_t *endAddr;
    };

} // namespace hooklib