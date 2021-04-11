#include <jni.h>
#include <android/log.h>
#include "libs/KittyMemory/MemoryPatch.h"
#include "libs/64InlineHook/And64InlineHook.hpp"
#include <memory.h>
#include <dlfcn.h>
#include <cstdio>
#include <cstdlib>
#include <include/utils.h>
#include <include/offsets.h>

#define LOG_TAG  "HOOK_LIBRARY"

//#define  LOGE(...)  __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)
//#define  LOGW(...)  __android_log_print(ANDROID_LOG_WARN,LOG_TAG,__VA_ARGS__)
#define  LOGD(...)  __android_log_print(ANDROID_LOG_DEBUG,LOG_TAG,__VA_ARGS__)
//#define  LOGI(...)  __android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__)

struct Il2CppObject {
    void *klass;
    void *monitor;
};

// System.String
struct Il2CppString {
    Il2CppObject object;
    int32_t length; // 0x10 < Length of string *excluding* the trailing null (which is included in 'chars').
    char16_t data[1]; // 0x14

    typedef Il2CppString *Il2CppString_CreateString(void *this_, const char *str);

    typedef Il2CppString *Il2CppString_Concat(Il2CppString *str0, Il2CppString *str1);

    static Il2CppString *New(const char *str) {
        return findFunction<Il2CppString_CreateString>(Offsets::Methods::String_Create)(nullptr,
                                                                                        str);
    }

    static Il2CppString *Concat(Il2CppString *str0, Il2CppString *str1) {
        return findFunction<Il2CppString_Concat>(Offsets::Methods::String_Concat)(str0, str1);
    }
};

float (*old_Player_getSkillCooldown)(void *instance);

float (Player_getSkillCooldown)(void *instance) {
    if (instance != NULL) {
        return 0;
    }
    return old_Player_getSkillCooldown(instance);
}

int (*old_Player_getWeaponDamage)(void *instance);

int Player_getWeaponDamage(void *instance) {
    if (instance != NULL) {
        return 1000000;
    }
    return old_Player_getWeaponDamage(instance);
}

__attribute__((constructor))
void libhook_main() {

    LOGD("Initialize hooking");

    while (libBase == 0) {
        libBase = get_libBase(libName);
        sleep(1);
    }

    LOGD("Apply hooks");

    //arm64 hook example.
    A64HookFunction((void *) getRealOffset(0x89552A), (void *) Player_getSkillCooldown,
                    (void **) &old_Player_getSkillCooldown);
    A64HookFunction((void *) getRealOffset(0x89556C), (void *) Player_getWeaponDamage,
                    (void **) &old_Player_getWeaponDamage);

    LOGD("Done, the game should now behave abnormally");
}
