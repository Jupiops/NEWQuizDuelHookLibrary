#include "Targets.h"
#include "core/OffsetRegistry.h"
#include "core/Logger.h"
#include "Il2CppTypes.h"
#include <cstring>

namespace targets {

    void (*orig_QuestionAnswerButton_Init)(void *, int32_t, il2cpp::String *, void *,
                                           const void *) = nullptr;

    float (*orig_QuestionContainerClassic_GetTimerDuration)(void *, const void *) = nullptr;

    bool (*orig_VIPManager_HasVIPProperty)(int32_t, const void *) = nullptr;

    static il2cpp::String *makeString(const char *utf8) {
        auto offCreate = hooklib::OffsetRegistry::instance().get("String_CreateString");
        if (!offCreate || *offCreate == 0) {
            LOGW("makeString: create offset missing");
            return nullptr;
        }
        // Example: base resolution done externally — adapt accordingly
        // This sample assumes you compute absolute address earlier and store separately if needed.
        auto fn = reinterpret_cast<il2cpp::String::CreateFn>(*offCreate); // Replace with real absolute pointer
        if (!fn) return nullptr;
        int len = (int) std::strlen(utf8);
        return fn(nullptr, utf8, 0, len);
    }

    static il2cpp::String *concat(il2cpp::String *a, il2cpp::String *b) {
        auto offConcat = hooklib::OffsetRegistry::instance().get("String_Concat");
        if (!offConcat || *offConcat == 0) return nullptr;
        auto fn = reinterpret_cast<il2cpp::String::ConcatFn>(*offConcat); // Replace with real absolute pointer
        return fn(a, b);
    }

    void QuestionAnswerButton_Init(void *self, int32_t answerIndex, il2cpp::String *text,
                                   void *onClick, const void *method) {
        if (answerIndex == 0 && text) {
            if (auto checkStr = makeString(" ✔")) {
                if (auto appended = concat(text, checkStr)) {
                    text = appended;
                }
            }
        }
        if (orig_QuestionAnswerButton_Init)
            orig_QuestionAnswerButton_Init(self, answerIndex, text, onClick, method);
    }

    float QuestionContainerClassic_GetTimerDuration(void *self, const void *method) {
        return 60.0f; // Example
    }

    bool VIPManager_HasVIPProperty(int32_t property, const void *method) {
        return true;
    }

} // namespace targets