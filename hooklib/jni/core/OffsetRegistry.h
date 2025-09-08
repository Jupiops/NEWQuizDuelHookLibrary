#pragma once

#include <unordered_map>
#include <string>
#include <cstdint>
#include <optional>
#include <mutex>

namespace hooklib {

    class OffsetRegistry {
    public:
        static OffsetRegistry &instance();

        void set(const std::string &name, uintptr_t offset);

        std::optional<uintptr_t> get(const std::string &name) const;

        void clear();

    private:
        OffsetRegistry() = default;

        mutable std::mutex mtx;
        std::unordered_map<std::string, uintptr_t> map;
    };

} // namespace hooklib