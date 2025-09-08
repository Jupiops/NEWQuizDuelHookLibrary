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

        // Stores an image-relative offset for a named symbol.
        void set(const std::string &name, uintptr_t offset);

        // Returns the image-relative offset (legacy behavior).
        std::optional <uintptr_t> get(const std::string &name) const;

        // Explicit relative getter (same as get()).
        std::optional <uintptr_t> getRelative(const std::string &name) const { return get(name); }

        // Image base management.
        void setImageBase(uintptr_t base);

        std::optional <uintptr_t> getImageBase() const;

        // Returns absolute address if both imageBase and relative offset are known.
        std::optional <uintptr_t> getAbsolute(const std::string &name) const;

        void clear();

    private:
        OffsetRegistry() = default;

        mutable std::mutex mtx;
        std::unordered_map <std::string, uintptr_t> map; // image-relative offsets
        uintptr_t imageBase_ = 0;
    };

} // namespace hooklib