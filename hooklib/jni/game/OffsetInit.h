#pragma once

#include "core/MemoryMap.h"

namespace game_offsets {

// Returns true if all mandatory patterns are found and offsets registered.
    bool InitializeOffsets(hooklib::MemoryMap &mmap);

} // namespace game_offsets