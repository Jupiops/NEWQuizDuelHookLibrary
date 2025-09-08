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
        auto absCreate = hooklib::OffsetRegistry::instance().getAbsolute("String_CreateString");
        if (!absCreate) {
            LOGW("makeString: String_CreateString absolute address missing");
            return nullptr;
        }
        auto fn = reinterpret_cast<il2cpp::String::CreateFn>(*absCreate);
        if (!fn) return nullptr;
        int len = (int) std::strlen(utf8);
        return fn(nullptr, utf8, 0, len);
    }

    static il2cpp::String *concat(il2cpp::String *a, il2cpp::String *b) {
        auto absConcat = hooklib::OffsetRegistry::instance().getAbsolute("String_Concat");
        if (!absConcat) {
            LOGW("concat: String_Concat absolute address missing");
            return nullptr;
        }
        auto fn = reinterpret_cast<il2cpp::String::ConcatFn>(*absConcat);
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
        return 60.0f;
    }

    bool VIPManager_HasVIPProperty(int32_t property, const void *method) {
        return true;
    }

} // namespace targets