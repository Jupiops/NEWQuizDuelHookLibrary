#pragma once

#include <cstdint>
#include "Il2CppTypes.h"

namespace targets {

// Forward declarations of detours:
    void QuestionAnswerButton_Init(void *this_, int32_t answerIndex, il2cpp::String *text,
                                   void *onClick, const void *method);

    float QuestionContainerClassic_GetTimerDuration(void *this_, const void *method);

    bool VIPManager_HasVIPProperty(int32_t property, const void *method);

// Original function pointers:
    extern void
    (*orig_QuestionAnswerButton_Init)(void *, int32_t, il2cpp::String *, void *, const void *);

    extern float (*orig_QuestionContainerClassic_GetTimerDuration)(void *, const void *);

    extern bool (*orig_VIPManager_HasVIPProperty)(int32_t, const void *);

} // namespace targets