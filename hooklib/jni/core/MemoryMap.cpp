#include "MemoryMap.h"
#include "Logger.h"
#include <cstdio>
#include <cstring>
#include <cinttypes>

namespace hooklib {

    bool MemoryMap::refresh() {
        FILE *fp = fopen("/proc/self/maps", "r");
        if (!fp) {
            LOGE("MemoryMap: cannot open /proc/self/maps");
            segs.clear();
            aggregateRange = {};
            return false;
        }

        segs.clear();
        aggregateRange = {};
        aggregateRange.base = UINTPTR_MAX;
        aggregateRange.end = 0;

        char line[768];
        bool foundAny = false;

        while (fgets(line, sizeof(line), fp)) {
            // We only care about lines that mention the library name
            if (!strstr(line, libName.c_str())) continue;

            // Example line:
            // 7a2b1a6000-7a2b3b9000 r--p 00000000 103:05 123456 /apex/.../libil2cpp.so
            uintptr_t start = 0, end = 0;
            char perms[8] = {};
            // Parse conservatively: addresses + perms
            if (sscanf(line, "%lx-%lx %7s", &start, &end, perms) != 3) continue;

            MapSegment seg{};
            seg.start = start;
            seg.end = end;
            seg.r = perms[0] == 'r';
            seg.w = perms[1] == 'w';
            seg.x = perms[2] == 'x';

            segs.push_back(seg);
            foundAny = true;

            if (start < aggregateRange.base) aggregateRange.base = start;
            if (end > aggregateRange.end) aggregateRange.end = end;
        }
        fclose(fp);

        if (!foundAny) {
            segs.clear();
            aggregateRange = {};
            return false;
        }
        if (aggregateRange.base == UINTPTR_MAX || aggregateRange.end <= aggregateRange.base) {
            LOGE("MemoryMap: inconsistent addresses for %s", libName.c_str());
            segs.clear();
            aggregateRange = {};
            return false;
        }

        // Debug log every segment for sanity
        for (auto &s: segs) {
            LOGD("MemoryMap: %s seg [%p - %p] %s%s%s",
                 libName.c_str(), (void *) s.start, (void *) s.end,
                 s.r ? "r" : "-", s.w ? "w" : "-", s.x ? "x" : "-");
        }
        LOGD("MemoryMap: %s imageBase=%p end=%p size=0x%lx",
             libName.c_str(), (void *) aggregateRange.base, (void *) aggregateRange.end,
             (unsigned long) (aggregateRange.end - aggregateRange.base));

        return true;
    }

} // namespace hooklib
