#include <jni.h>
#include <thread>
#include <chrono>
#include <dlfcn.h>
#include "core/Logger.h"
#include "core/MemoryMap.h"
#include "core/HookManager.h"
#include "core/OffsetRegistry.h"
#include "arch/arm64/HookBackend.h"
#include "arch/armv7/HookBackend.h"
#include "game/Targets.h"
#include "game/OffsetInit.h"

namespace {

    constexpr const char *kTargetLib = "libil2cpp.so";

#if defined(__aarch64__)
    using ActiveBackend = hooklib::HookBackendArm64;
#else
    using ActiveBackend = hooklib::HookBackendArmv7;
#endif

    void bootstrap() {
        LOGI("Bootstrap thread started");
        hooklib::MemoryMap mmap(kTargetLib);

        // Structured wait with exponential backoff
        int attempts = 0;
        const int maxAttempts = 60;
        std::chrono::milliseconds delay(50);

        while (attempts < maxAttempts) {
            if (mmap.refresh()) break;
            std::this_thread::sleep_for(delay);
            if (delay < std::chrono::milliseconds(1000))
                delay *= 2;
            ++attempts;
        }
        if (!mmap.range().valid()) {
            LOGE("Failed to locate %s after %d attempts", kTargetLib, attempts);
            return;
        }

        // Initialize offsets (also sets imageBase in OffsetRegistry)
        if (!game_offsets::InitializeOffsets(mmap)) {
            LOGE("Mandatory offsets missing; aborting hook install.");
            return;
        }

        ActiveBackend backend;
        hooklib::HookManager hm(backend, mmap.imageBase());

        // Build hook requests referencing offset names
        std::vector<hooklib::HookRequest> reqs = {
                {"QuestionAnswerButton_Init",                 "QuestionAnswerButton_Init",                 (void *) targets::QuestionAnswerButton_Init,                 (void **) &targets::orig_QuestionAnswerButton_Init},
                {"QuestionContainerClassic_GetTimerDuration", "QuestionContainerClassic_GetTimerDuration", (void *) targets::QuestionContainerClassic_GetTimerDuration, (void **) &targets::orig_QuestionContainerClassic_GetTimerDuration},
                {"VIPManager_HasVIPProperty",                 "VIPManager_HasVIPProperty",                 (void *) targets::VIPManager_HasVIPProperty,                 (void **) &targets::orig_VIPManager_HasVIPProperty},
                {"VIPManager_HasGeneralVIP",                  "VIPManager_HasGeneralVIP",                  (void *) targets::VIPManager_HasGeneralVIP,                  (void **) &targets::orig_VIPManager_HasGeneralVIP},
                {"CurrencyManager_GetBalance",                "CurrencyManager_GetBalance",                (void *) targets::CurrencyManager_GetBalance,                (void **) &targets::orig_CurrencyManager_GetBalance},
        };

        for (auto &r: reqs) {
            auto res = hm.install(r);
            LOGI("Hook %s -> %s", r.hookName.c_str(),
                 res.status == hooklib::HookInstallStatus::Success ? "OK" : "FAIL");
        }

        LOGI("Bootstrap complete.");
    }

} // anonymous namespace

JNIEXPORT jint

JNICALL JNI_OnLoad(JavaVM *vm, void *) {
    (void) vm;
    std::thread(bootstrap).detach();
    return JNI_VERSION_1_6;
}