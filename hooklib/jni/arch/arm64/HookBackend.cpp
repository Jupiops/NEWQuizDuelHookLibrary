#include "HookBackend.h"
#include "Logger.h"
#include <cstring>

// Include And64InlineHook
#include <libs/And64InlineHook/And64InlineHook.hpp>

namespace hooklib {

// If you use a fork where A64HookFunction returns int, define HOOKLIB_A64_RETURNS_INT=1
// (e.g., add -DHOOKLIB_A64_RETURNS_INT=1 to LOCAL_CPPFLAGS) to enable return-code checking.

    HookInstallStatus HookBackendArm64::hook(void *target, void *detour, void **original) {
        if (!target || !detour) return HookInstallStatus::Failure_InvalidTarget;

#if defined(HOOKLIB_A64_RETURNS_INT) && HOOKLIB_A64_RETURNS_INT
        int rc = A64HookFunction(target, detour, original);
    if (rc != 0) {
        LOGE("Arm64 backend: A64HookFunction failed rc=%d", rc);
        return HookInstallStatus::Failure_BackendError;
    }
    return HookInstallStatus::Success;
#else
        // And64InlineHook in this repo returns void
        void *before = (original ? *original : nullptr);
        A64HookFunction(target, detour, original);

        // Optional heuristic: if a trampoline pointer is expected, verify it changed
        if (original && *original && *original != before) {
            return HookInstallStatus::Success;
        }
        // Many implementations still succeed even if original remains null (no trampoline).
        // Treat as success unless you specifically require a trampoline pointer.
        return HookInstallStatus::Success;
#endif
    }

} // namespace hooklib