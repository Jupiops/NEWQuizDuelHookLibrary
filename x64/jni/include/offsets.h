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
         * [Address(RVA = "0xD6F8E0", Offset = "0xD6F8E0", VA = "0xD6F8E0")]
         * public void QuizDuel.QuestionAnswerButton.Init(int answerIndex, string text, ButtonClick onClick)
         */
        DWORD QuestionAnswerButton_Init;
        /*
         * [Address(RVA = "0xD9C06C", Offset = "0xD9C06C", VA = "0xD9C06C", Slot = "25")]
         * protected override float QuizDuel.Classic.QuestionContainerClassic.GetTimerDuration()
         */
        DWORD QuestionContainerClassic_GetTimerDuration;
        /*
         * [Address(RVA = "0xD1B010", Offset = "0xD1B010", VA = "0xD1B010")]
         * public static bool VIPManager.HasVIPProperty(VIPProperty property)
         */
        DWORD VIPManager_HasVIPProperty;
        /*
         * [Address(RVA = "0x1563C0C", Offset = "0x1563C0C", VA = "0x1563C0C")]
         * private unsafe string string.CreateString(sbyte* value)
         */
        DWORD String_CreateString;
        /*
         * [Address(RVA = "0x1557674", Offset = "0x1557674", VA = "0x1557674")]
         * public static string string.Concat(string str0, string str1)
         */
        DWORD String_Concat;
    }

    void Initialize() {
        Methods::QuestionAnswerButton_Init = 0xD6F8E0;
        Methods::QuestionContainerClassic_GetTimerDuration = 0xD9C06C;
        Methods::VIPManager_HasVIPProperty = 0xD1B010;

        Methods::String_CreateString = 0x1563C0C;
        Methods::String_Concat = 0x1557674;
    }
}

#endif //OFFSETS_H