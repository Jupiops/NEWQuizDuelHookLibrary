#pragma once

#include "core/HookBackendIface.h"

namespace hooklib {

    class HookBackendArmv7 : public IHookBackend {
    public:
        HookInstallStatus hook(void *target, void *detour, void **original) override;
    };

} // namespace hooklib