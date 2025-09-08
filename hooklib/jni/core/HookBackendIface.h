#pragma once

#include <cstdint>
#include <string>

namespace hooklib {

    enum class HookInstallStatus {
        Success,
        Failure_InvalidTarget,
        Failure_BackendError
    };

    class IHookBackend {
    public:
        virtual ~IHookBackend() = default;

        virtual HookInstallStatus hook(void *target, void *detour, void **original) = 0;
    };

} // namespace hooklib