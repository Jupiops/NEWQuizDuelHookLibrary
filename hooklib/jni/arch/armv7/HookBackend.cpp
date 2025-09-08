#include "HookBackend.h"
#include "Logger.h"
#include <libs/Substrate/CydiaSubstrate.h>   // Adjust include path as needed

namespace hooklib {

    static void substrateTrampoline(void *target, void *detour, void **original) {
        MSHookFunction(target, detour, original);
    }

    HookInstallStatus HookBackendArmv7::hook(void *target, void *detour, void **original) {
        if (!target || !detour) return HookInstallStatus::Failure_InvalidTarget;
        // Substrate typically has no return error code; we assume success if not crash:
        substrateTrampoline(target, detour, original);
        return HookInstallStatus::Success;
    }

} // namespace hooklib