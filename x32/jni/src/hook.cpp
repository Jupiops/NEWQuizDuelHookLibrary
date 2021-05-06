#include <jni.h>
#include <android/log.h>
#include <libs/KittyMemory/MemoryPatch.h>
#include <libs/Substrate/CydiaSubstrate.h>
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
    __unused void *klass;
    __unused void *monitor;
};

// System.String
struct Il2CppString {
    __unused Il2CppObject
    object;
    int32_t length; // 0x10 < Length of string *excluding* the trailing null (which is included in 'chars').
    char16_t data[1]; // 0x14

    typedef Il2CppString *Il2CppString_CreateString(void *this_, const char *str);

    typedef Il2CppString *Il2CppString_Concat(Il2CppString *str0, Il2CppString *str1);

    static Il2CppString *New(const char *str) {
        return findFunction<Il2CppString_CreateString>(Offsets::Methods::String_CreateString)(nullptr, str);
    }

    static Il2CppString *Concat(Il2CppString *str0, Il2CppString *str1) {
        return findFunction<Il2CppString_Concat>(Offsets::Methods::String_Concat)(str0, str1);
    }
};

void (*old_QuestionAnswerButton_Init)(void *this_, int32_t answerIndex, Il2CppString *text, void *onClick,
                                      const void *method);

void
QuestionAnswerButton_Init(void *this_, int32_t answerIndex, Il2CppString *text, void *onClick, const void *method) {
    if (answerIndex == 0) {
        text = Il2CppString::Concat(text, Il2CppString::New(" ✔"));
    }
    old_QuestionAnswerButton_Init(this_, answerIndex, text, onClick, method);
}

float (*old_QuestionContainerClassic_GetTimerDuration)(void *this_, const void *method);

float QuestionContainerClassic_GetTimerDuration(__unused void *this_, __unused const void *method) {
    return 60;
}

bool (*old_VIPManager_HasVIPProperty)(int32_t property, const void *method);

bool VIPManager_HasVIPProperty(__unused int32_t property, __unused const void *method) {
    return true;
}

void *libhook_main(void *) {

    LOGD("Initialize hooking");

    // loop until our target library is found
    ProcMap il2cppMap;
    do {
        il2cppMap = KittyMemory::getLibraryMap(libName);
        sleep(1);
    } while (!il2cppMap.isValid());;

    libBase = (DWORD) il2cppMap.startAddr;
    libEnd = (DWORD) il2cppMap.endAddr;

    Offsets::Initialize();

    LOGD("QuestionAnswerButton.Init Offset: %p", (void *) Offsets::Methods::QuestionAnswerButton_Init);
    LOGD("QuestionContainerClassic.GetTimerDuration Offset: %p",
         (void *) Offsets::Methods::QuestionContainerClassic_GetTimerDuration);
    LOGD("VIPManager.HasVIPProperty Offset: %p", (void *) Offsets::Methods::VIPManager_HasVIPProperty);
    LOGD("string.CreateString Offset: %p", (void *) Offsets::Methods::String_CreateString);
    LOGD("string.Concat Offset: %p", (void *) Offsets::Methods::String_Concat);

    LOGD("Apply hooks");

    MSHookFunction((void *) getRealOffset(Offsets::Methods::QuestionAnswerButton_Init),
                   (void *) QuestionAnswerButton_Init,
                   (void **) &old_QuestionAnswerButton_Init);

    MSHookFunction(
            (void *) getRealOffset(Offsets::Methods::QuestionContainerClassic_GetTimerDuration),
            (void *) QuestionContainerClassic_GetTimerDuration,
            (void **) &old_QuestionContainerClassic_GetTimerDuration);

//    MSHookFunction((void *) getRealOffset(Offsets::Methods::VIPManager_HasVIPProperty),
//                   (void *) VIPManager_HasVIPProperty,
//                   (void **) &old_VIPManager_HasVIPProperty);

    LOGD("Done, the game should now behave abnormally");
    return nullptr;
}

JNIEXPORT jint JNICALL
JNI_OnLoad(JavaVM *vm, __unused void *reserved) {
    JNIEnv *globalEnv;
    vm->GetEnv((void **) &globalEnv, JNI_VERSION_1_6);

    // Create a new thread so it does not block the main thread, means the game would not freeze
    pthread_t ptid;
    pthread_create(&ptid, nullptr, libhook_main, nullptr);

    return JNI_VERSION_1_6;
}

JNIEXPORT void JNICALL
JNI_OnUnload(__unused JavaVM *vm, __unused void *reserved) {}