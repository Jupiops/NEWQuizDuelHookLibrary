#pragma once

#include <cstdint>
#include <string>
#include <optional>
#include <vector>

namespace hooklib {

    struct LibraryRange {
        uintptr_t base = 0;
        uintptr_t end = 0;

        bool valid() const noexcept { return base != 0 && end > base; }
    };

    struct MapSegment {
        uintptr_t start = 0;
        uintptr_t end = 0;
        bool r = false, w = false, x = false;

        bool readable() const noexcept { return r; }

        bool executable() const noexcept { return x; }
    };

    class MemoryMap {
    public:
        explicit MemoryMap(std::string lib) : libName(std::move(lib)) {}

        bool refresh();                        // Re-scan /proc/self/maps
        LibraryRange range() const noexcept { return aggregateRange; }

        const std::string &libraryName() const noexcept { return libName; }

        // NEW:
        const std::vector<MapSegment> &segments() const noexcept { return segs; }

        uintptr_t imageBase() const noexcept { return aggregateRange.base; } // lowest segment start

    private:
        std::string libName;
        LibraryRange aggregateRange{};
        std::vector<MapSegment> segs;
    };

} // namespace hooklib
