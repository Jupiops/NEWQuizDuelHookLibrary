#pragma once

#include "core/MemoryMap.h"
#include "core/PatternScanner.h"

namespace hooklib {

    inline PatternResult ScanPatternInLibrary(const MemoryMap &mm,
                                              const PatternSpec &spec) {
        PatternResult best{PatternStatus::NotFound, 0, "No match", {}};
        for (const auto &seg: mm.segments()) {
            if (!seg.readable()) continue;              // only readable segments
            uint8_t *base = reinterpret_cast<uint8_t *>(seg.start);
            uint8_t *end = reinterpret_cast<uint8_t *>(seg.end);
            PatternScanner ps(base, end);
            auto r = ps.find(spec);
            if (r.status == PatternStatus::Found) {
                LOGD("Pattern %s found in segment 0x%lx-0x%lx at offset 0x%lx",
                     spec.name.c_str(),
                     (unsigned long) seg.start,
                     (unsigned long) seg.end,
                     (unsigned long) r.relativeOffset);
                // Convert from "segment-relative" to "image-relative"
                r.relativeOffset += (seg.start - mm.imageBase());
                return r; // return first match; or keep searching if you expect multiple
            }
            if (r.status == PatternStatus::ErrorPatternFormat) return r;
            if (best.status != PatternStatus::Found) best = r;
        }
        return best;
    }

} // namespace hooklib
