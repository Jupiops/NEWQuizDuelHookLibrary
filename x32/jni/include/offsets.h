//
// Created by Jupio on 10.04.2021.
//
#ifndef OFFSETS_H
#define OFFSETS_H

#include "utils.h"

namespace Offsets {
    namespace Classes {

    }
    namespace Methods {
        /*
         * [Address(RVA = "0x674980", Offset = "0x674980", VA = "0x674980")]
         * public void QuizDuel.QuestionAnswerButton.Init(int answerIndex, string text, ButtonClick onClick)
         */
        DWORD QuestionAnswerButton_Init;
        /*
         * [Address(RVA = "0x6A9FC4", Offset = "0x6A9FC4", VA = "0x6A9FC4", Slot = "25")]
         * protected override float QuizDuel.Classic.QuestionContainerClassic.GetTimerDuration()
         */
        DWORD QuestionContainerClassic_GetTimerDuration;
        /*
         * [Address(RVA = "0x60C040", Offset = "0x60C040", VA = "0x60C040")]
         * public static bool VIPManager.HasVIPProperty(VIPProperty property)
         */
        DWORD VIPManager_HasVIPProperty;
        /*
         * [Address(RVA = "0x1020700", Offset = "0x1020700", VA = "0x1020700")]
         * private unsafe string string.CreateString(sbyte* value)
         */
        DWORD String_CreateString;
        /*
         * [Address(RVA = "0x1011810", Offset = "0x1011810", VA = "0x1011810")]
         * public static string string.Concat(string str0, string str1)
         */
        DWORD String_Concat;
    }

    void Initialize() {
        Methods::QuestionAnswerButton_Init = 0x674980;
        Methods::QuestionContainerClassic_GetTimerDuration = 0x6A9FC4;
        Methods::VIPManager_HasVIPProperty = 0x60C040;

        Methods::String_CreateString = 0x1020700;
        Methods::String_Concat = 0x1011810;
    }
}

#endif //OFFSETS_H
