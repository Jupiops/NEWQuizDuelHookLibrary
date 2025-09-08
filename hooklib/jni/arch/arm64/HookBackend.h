#pragma once

#include "core/HookBackendIface.h"

namespace hooklib {

    class HookBackendArm64 : public IHookBackend {
    public:
        HookInstallStatus hook(void *target, void *detour, void **original) override;
    };

} // namespace hooklib