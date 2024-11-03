#include <libs/KittyMemory/MemoryPatch.h>
#include <libs/64InlineHook/And64InlineHook.hpp>
#include <memory.h>
#include <dlfcn.h>
#include <cstdlib>
#include "include/offsets.h"

#if defined(_MSC_VER)
    #define IL2CPP_ZERO_LEN_ARRAY 0
#else
    #define IL2CPP_ZERO_LEN_ARRAY 0
#endif

#if _MSC_VER
typedef wchar_t Il2CppChar;
#elif __has_feature(cxx_unicode_literals)
typedef char16_t Il2CppChar;
#else
typedef uint16_t Il2CppChar;
#endif

typedef struct Il2CppObject
{
    union
    {
        __unused void *klass;
        __unused void *vtable;
    };
    __unused void *monitor;
} Il2CppObject;


// System.String
typedef struct Il2CppString
{
    Il2CppObject object;
    int32_t length;                             ///< Length of string *excluding* the trailing null (which is included in 'chars').
    Il2CppChar chars[IL2CPP_ZERO_LEN_ARRAY];

    // typedef Il2CppString *Il2CppString_CreateString(void *this_, const char *str);
    typedef Il2CppString *Il2CppString_CreateString(void *this_, const char *str, int startIndex, int length);

    typedef Il2CppString *Il2CppString_Concat(Il2CppString *str0, Il2CppString *str1);

    // static Il2CppString *New(const char *str) {
    //     return findFunction<Il2CppString_CreateString>(Offsets::Methods::String_CreateString)(nullptr, str);
    // }
    static Il2CppString *New(const char *str) {
        int length = strlen(str);
        // LOGD("Creating new string \"%s\" with length %d", str, length);
        return findFunction<Il2CppString_CreateString>(Offsets::Methods::String_CreateString)(nullptr, str, 0, length);
    }

    static Il2CppString *Concat(Il2CppString *str0, Il2CppString *str1) {
        // LOGD("Length of string 0: %d, length of string 1: %d", str0->length, str1->length);
        return findFunction<Il2CppString_Concat>(Offsets::Methods::String_Concat)(str0, str1);
    }

} Il2CppString;


void (*old_QuestionAnswerButton_Init)(void *this_, int32_t answerIndex, Il2CppString *text,
                                      void *onClick, const void *method);

void QuestionAnswerButton_Init(void *this_, int32_t answerIndex, Il2CppString *text, void *onClick,
                               const void *method) {
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
    LOGD("Initialize hooking for x64");

    // loop until our target library is found
    ProcMap il2cppMap;
    LOGD("Searching for target library: %s", libraryName);

    while (true) {
        il2cppMap = KittyMemory::getLibraryMap(libraryName);
        if (il2cppMap.isValid()) {
            LOGD("KittyMemory: Library %s found at address range: %p - %p", libraryName,
                 (void *) il2cppMap.startAddr, (void *) il2cppMap.endAddr);
            break;
        }
        LOGW("Library %s not found, retrying...", libraryName);
        sleep(1);
    }

    getLibraryBase(libraryName);
    LOGD("Utils: Library %s found at address range: %p - %p", libraryName, (void *) libBase,
         (void *) libEnd);

//    libBase = (uintptr_t) il2cppMap.startAddr;
//    libEnd = (uintptr_t) il2cppMap.endAddr;

    // Initialize offsets and log if initialization fails
    LOGD("Initializing offsets...");
    Offsets::Initialize();

    LOGD("Offsets initialized.");

    // Log the offsets and check for invalid offsets
    if (Offsets::Methods::QuestionAnswerButton_Init == 0) {
        LOGE("QuestionAnswerButton_Init offset is 0, this is invalid!");
    } else {
        LOGD("QuestionAnswerButton.Init Offset: %p",
             (void *) Offsets::Methods::QuestionAnswerButton_Init);
    }

    if (Offsets::Methods::QuestionContainerClassic_GetTimerDuration == 0) {
        LOGE("QuestionContainerClassic_GetTimerDuration offset is 0, this is invalid!");
    } else {
        LOGD("QuestionContainerClassic.GetTimerDuration Offset: %p",
             (void *) Offsets::Methods::QuestionContainerClassic_GetTimerDuration);
    }

    if (Offsets::Methods::VIPManager_HasVIPProperty == 0) {
        LOGE("VIPManager_HasVIPProperty offset is 0, this is invalid!");
    } else {
        LOGD("VIPManager.HasVIPProperty Offset: %p",
             (void *) Offsets::Methods::VIPManager_HasVIPProperty);
    }

    if (Offsets::Methods::String_CreateString == 0) {
        LOGE("String_CreateString offset is 0, this is invalid!");
    } else {
        LOGD("string.CreateString Offset: %p", (void *) Offsets::Methods::String_CreateString);
    }

    if (Offsets::Methods::String_Concat == 0) {
        LOGE("String_Concat offset is 0, this is invalid!");
    } else {
        LOGD("string.Concat Offset: %p", (void *) Offsets::Methods::String_Concat);
    }

    // Apply hooks and log each step
    LOGD("Applying hooks...");

    if (Offsets::Methods::QuestionAnswerButton_Init != 0) {
        A64HookFunction((void *) getRealOffset(Offsets::Methods::QuestionAnswerButton_Init),
                        (void *) QuestionAnswerButton_Init,
                        (void **) &old_QuestionAnswerButton_Init);
        LOGD("Hook applied for QuestionAnswerButton_Init");
    } else {
        LOGE("Failed to apply hook for QuestionAnswerButton_Init due to invalid offset.");
    }

    if (Offsets::Methods::QuestionContainerClassic_GetTimerDuration != 0) {
        A64HookFunction(
                (void *) getRealOffset(Offsets::Methods::QuestionContainerClassic_GetTimerDuration),
                (void *) QuestionContainerClassic_GetTimerDuration,
                (void **) &old_QuestionContainerClassic_GetTimerDuration);
        LOGD("Hook applied for QuestionContainerClassic_GetTimerDuration");
    } else {
        LOGE("Failed to apply hook for QuestionContainerClassic_GetTimerDuration due to invalid offset.");
    }

    if (Offsets::Methods::VIPManager_HasVIPProperty != 0) {
        A64HookFunction((void *) getRealOffset(Offsets::Methods::VIPManager_HasVIPProperty),
                        (void *) VIPManager_HasVIPProperty,
                        (void **) &old_VIPManager_HasVIPProperty);
        LOGD("Hook applied for VIPManager_HasVIPProperty");
    } else {
        LOGE("Failed to apply hook for VIPManager_HasVIPProperty due to invalid offset.");
    }

    LOGD("Done, the game should now behave abnormally (hooks applied)");
    return nullptr;
}

JNIEXPORT jint

JNICALL
JNI_OnLoad(JavaVM *vm, __unused void *reserved) {
    JNIEnv *globalEnv;
    vm->GetEnv((void **) &globalEnv, JNI_VERSION_1_6);

    // Create a new thread so it does not block the main thread, means the game would not freeze
    pthread_t ptid;
    pthread_create(&ptid, nullptr, libhook_main, nullptr);

    return JNI_VERSION_1_6;
}

JNIEXPORT void JNICALL
JNI_OnUnload(__unused
JavaVM *vm, __unused
void *reserved
) {
}