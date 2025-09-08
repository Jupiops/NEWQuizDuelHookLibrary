#pragma once

#include "HookBackendIface.h"
#include <vector>
#include <string>
#include <functional>
#include <cstdint>

namespace hooklib {

    struct HookRequest {
        std::string hookName;
        std::string offsetName;   // key in OffsetRegistry (image-relative)
        void *detour;
        void **original;
    };

    struct HookResult {
        HookRequest req;
        HookInstallStatus status;
        uintptr_t resolvedAddress = 0; // absolute address used for hook
        std::string message;
    };

    class HookManager {
    public:
        // Provide imageBase explicitly to guarantee rel->abs conversion.
        explicit HookManager(IHookBackend &backend, uintptr_t imageBase)
                : backend(backend), imageBase(imageBase) {}

        HookResult install(const HookRequest &req);

        static uintptr_t realAddress(uintptr_t relative, uintptr_t base)

        noexcept {
            return base + relative;
        }

    private:
        IHookBackend &backend;
        uintptr_t imageBase = 0;
    };

} // namespace hooklib