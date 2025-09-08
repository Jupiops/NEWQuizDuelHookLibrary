#pragma once

#include "HookBackendIface.h"
#include <vector>
#include <string>
#include <functional>

namespace hooklib {

    struct HookRequest {
        std::string hookName;
        std::string offsetName;   // key in OffsetRegistry
        void *detour;
        void **original;
    };

    struct HookResult {
        HookRequest req;
        HookInstallStatus status;
        uintptr_t resolvedAddress = 0;
        std::string message;
    };

    class HookManager {
    public:
        explicit HookManager(IHookBackend &backend) : backend(backend) {}

        HookResult install(const HookRequest &req);

        static uintptr_t realAddress(uintptr_t relative, uintptr_t base) noexcept {
            return base + relative;
        }

    private:
        IHookBackend &backend;
    };

} // namespace hooklib