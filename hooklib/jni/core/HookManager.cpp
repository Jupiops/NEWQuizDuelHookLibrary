#include "HookManager.h"
#include "OffsetRegistry.h"
#include "Logger.h"

namespace hooklib {

    HookResult HookManager::install(const HookRequest &req) {
        HookResult result{req, HookInstallStatus::Failure_InvalidTarget, 0, {}};

        auto relOpt = OffsetRegistry::instance().get(req.offsetName);
        if (!relOpt || *relOpt == 0) {
            result.message = "Offset not found or zero";
            LOGE("HookManager: %s failed (offset missing)", req.hookName.c_str());
            return result;
        }

        uintptr_t absAddr = 0;
        if (imageBase != 0) {
            absAddr = imageBase + *relOpt;
        } else {
            auto absOpt = OffsetRegistry::instance().getAbsolute(req.offsetName);
            if (!absOpt) {
                result.message = "Image base not set";
                LOGE("HookManager: %s failed (image base not set)", req.hookName.c_str());
                return result;
            }
            absAddr = *absOpt;
        }

        result.resolvedAddress = absAddr;
        void *target = reinterpret_cast<void *>(absAddr);
        auto st = backend.hook(target, req.detour, req.original);
        result.status = st;

        switch (st) {
            case HookInstallStatus::Success:
                result.message = "Success";
                LOGI("Hook %s installed at abs %p (rel 0x%lx)",
                     req.hookName.c_str(), (void *) absAddr, (unsigned long) *relOpt);
                break;
            case HookInstallStatus::Failure_InvalidTarget:
                result.message = "Invalid target";
                LOGE("Hook %s invalid target (abs %p)", req.hookName.c_str(), (void *) absAddr);
                break;
            case HookInstallStatus::Failure_BackendError:
                result.message = "Backend error";
                LOGE("Hook %s backend error (abs %p)", req.hookName.c_str(), (void *) absAddr);
                break;
        }
        return result;
    }

} // namespace hooklib