#include "HookManager.h"
#include "OffsetRegistry.h"
#include "Logger.h"

namespace hooklib {

    HookResult HookManager::install(const HookRequest &req) {
        HookResult result{req, HookInstallStatus::Failure_InvalidTarget, 0, {}};
        auto offOpt = OffsetRegistry::instance().get(req.offsetName);
        if (!offOpt || *offOpt == 0) {
            result.message = "Offset not found or zero";
            LOGE("HookManager: %s failed (offset missing)", req.hookName.c_str());
            return result;
        }
        // base is obtained outside; we only set final address at call site
        // This example expects caller to supply final target pointer.
        // But we can embed base retrieval here if we pass MemoryMap.

        // For demonstration assume caller passes already converted absolute
        // In practice adapt to your architecture step: 
        // result.resolvedAddress = HookManager::realAddress(*offOpt, base); (call site)
        result.resolvedAddress = *offOpt; // relative for now
        void *target = reinterpret_cast<void *>(result.resolvedAddress);
        if (!target) {
            result.message = "Computed target null";
            return result;
        }
        auto st = backend.hook(target, req.detour, req.original);
        result.status = st;
        switch (st) {
            case HookInstallStatus::Success:
                result.message = "Success";
                LOGI("Hook %s installed at relative 0x%lx", req.hookName.c_str(),
                     (unsigned long) *offOpt);
                break;
            case HookInstallStatus::Failure_InvalidTarget:
                result.message = "Invalid target";
                LOGE("Hook %s invalid target", req.hookName.c_str());
                break;
            case HookInstallStatus::Failure_BackendError:
                result.message = "Backend error";
                LOGE("Hook %s backend error", req.hookName.c_str());
                break;
        }
        return result;
    }

} // namespace hooklib