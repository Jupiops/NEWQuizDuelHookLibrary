#include "OffsetInit.h"
#include "core/PatternScanner.h"
#include "core/OffsetRegistry.h"
#include "core/Logger.h"
#include "core/MemoryMap.h"
#include "PatternScanUtil.h"
#include <vector>

// Register and scan patterns.
// Replace the placeholder patterns with full real patterns (no "[...]").
namespace game_offsets {

    struct NamedPattern {
        hooklib::PatternSpec spec;
        bool mandatory = true;
    };

    static std::vector<NamedPattern> buildPatternList() {
        return {
                {{"QuestionAnswerButton_Init",                 "FE 0F 1C F8 F8 5F 01 A9 F6 57 02 A9 F4 4F 03 A9 ? ? 01 ? E8 ? ? 39 F6 03 03 AA F5 03 02 AA",                                                                                                                          false, 0}, true},
                {{"QuestionContainerClassic_GetTimerDuration", "FE 0F 1E F8 F4 4F 01 A9 ? ? 01 ? ? ? ? ? 68 ? ? 39 94 ? ? F9 C8 00 00 37 ? ? ? ? 00 ? ? F9 ? ? ? 97 28 00 80 52 68 ? ? 39 80 02 40 F9 08 E0 40 B9 48 00 00 35 ? ? ? 97 E0 03 1F AA ? ? ? 97 F4 4F 41 A9 1F 00 00 72", false, 0}, true},
                {{"String_CreateString",                       "E0 03 01 AA E1 03 02 2A E2 03 03 2A 13 FF FF 17 ? ? ? 17 FE 57 BE A9 F4 4F 01 A9 ? ? 00 ?",                                                                                                                           false, 0}, true},
                {{"String_Concat",                             "FE 0F 1D F8 F6 57 01 A9 F4 4F 02 A9 ? ? 00 ? A8 ? ? 39 F4 03 01 AA F3 03 00 AA C8 00 00 37 ? ? 00 ? 00 ? ? F9 ? ? ? 97 28 00 80 52 A8 ? ? 39 D3 00 00 B4",                                                            false, 0}, true},
                {{"VIPManager_HasVIPProperty",                 "1F 04 00 71 E1 00 00 54 FE 0F 1F F8 ? ? 00 94 FE 07 41 F8 60 00 00 36 20 00 80 52 C0 03 5F D6",                                                                                                                       false, 0}, true},
                {{"VIPManager_HasGeneralVIP",                  "FE 0F 1E F8 F4 4F 01 A9 ? ? 01 ? ? ? ? ? 68 ? ? 39 94 ? ? F9 C8 00 00 37 ? ? ? ? 00 ? ? F9 ? ? ? 97 28 00 80 52 68 ? ? 39 ? 00 00 94 F3 03 00 AA",                                                                    false, 0}, true},
                {{"CurrencyManager_GetBalance",                "FF 43 01 D1 FE 0B 00 F9 F8 5F 02 A9 F6 57 03 A9 F4 4F 04 A9 ? ? 01 ? C8 ? ? 39 F5 03 02 2A",                                                                                                                          false, 0}, true},
                // {{"", "", false, 0}, true},
        };
    }

    bool InitializeOffsets(hooklib::MemoryMap &mmap) {
        if (!mmap.range().valid()) {
            LOGE("InitializeOffsets: library range invalid");
            return false;
        }

        // Make imageBase available globally for absolute resolution later.
        hooklib::OffsetRegistry::instance().setImageBase(mmap.imageBase());

        auto patterns = buildPatternList();
        bool allMandatoryOk = true;

        for (auto &np: patterns) {
            auto res = hooklib::ScanPatternInLibrary(mmap, np.spec);
            if (res.status == hooklib::PatternStatus::Found) {
                hooklib::OffsetRegistry::instance().set(np.spec.name, res.relativeOffset);
                LOGI("Pattern %s found at rel 0x%lx (t=%lldus)",
                     np.spec.name.c_str(),
                     (unsigned long) res.relativeOffset,
                     (long long) res.duration.count());
            } else {
                LOGW("Pattern %s not found (%s)", np.spec.name.c_str(), res.message.c_str());
                if (np.mandatory) allMandatoryOk = false;
            }
        }
        return allMandatoryOk;
    }

} // namespace game_offsets