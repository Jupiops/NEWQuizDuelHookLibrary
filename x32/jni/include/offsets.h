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
         * android:versionCode="115020157" android:versionName="1.15.2"
         * [Address(RVA = "0x67B048", Offset = "0x67B048", VA = "0x67B048")]
         *
         * android:versionCode="114080151" android:versionName="1.14.8"
         * [Address(RVA = "0x674980", Offset = "0x674980", VA = "0x674980")]
         *
         * android:versionCode="112060129" android:versionName="1.12.6"
         * [Address(RVA = "0x608AC8", Offset = "0x608AC8", VA = "0x608AC8")]
         *
         * public void QuizDuel.QuestionAnswerButton.Init(int answerIndex, string text, ButtonClick onClick)
         */
        uintptr_t QuestionAnswerButton_Init;
        /*
         * android:versionCode="115020157" android:versionName="1.15.2"
         * [Address(RVA = "0x7AF6F8", Offset = "0x7AF6F8", VA = "0x7AF6F8", Slot = "25")]
         *
         * android:versionCode="114080151" android:versionName="1.14.8"
         * [Address(RVA = "0x6A9FC4", Offset = "0x6A9FC4", VA = "0x6A9FC4", Slot = "25")]
         *
         * android:versionCode="112060129" android:versionName="1.12.6"
         * [Address(RVA = "0x6B9A4C", Offset = "0x6B9A4C", VA = "0x6B9A4C", Slot = "25")]
         *
         * protected override float QuizDuel.Classic.QuestionContainerClassic.GetTimerDuration()
         */
        uintptr_t QuestionContainerClassic_GetTimerDuration;
        /*
         * android:versionCode="115020157" android:versionName="1.15.2"
         * [Address(RVA = "0x62BBD4", Offset = "0x62BBD4", VA = "0x62BBD4")]
         *
         * android:versionCode="114080151" android:versionName="1.14.8"
         * [Address(RVA = "0x60C040", Offset = "0x60C040", VA = "0x60C040")]
         *
         * android:versionCode="112060129" android:versionName="1.12.6"
         * [Address(RVA = "0x5FA284", Offset = "0x5FA284", VA = "0x5FA284")]
         *
         * public static bool VIPManager.HasVIPProperty(VIPProperty property)
         */
        uintptr_t VIPManager_HasVIPProperty;
        /*
         * android:versionCode="115020157" android:versionName="1.15.2"
         * [Address(RVA = "0x1043CC0", Offset = "0x1043CC0", VA = "0x1043CC0")]
         *
         * android:versionCode="114100153" android:versionName="1.14.10"
         * [Address(RVA = "0x1022770", Offset = "0x1022770", VA = "0x1022770")]
         *
         * android:versionCode="114080151" android:versionName="1.14.8"
         * [Address(RVA = "0x1020700", Offset = "0x1020700", VA = "0x1020700")]
         *
         * android:versionCode="112060129" android:versionName="1.12.6"
         * [Address(RVA = "0xFFDE38", Offset = "0xFFDE38", VA = "0xFFDE38")]
         *
         * private unsafe string string.CreateString(sbyte* value)
         */
        uintptr_t String_CreateString;
        /*
         * android:versionCode="115020157" android:versionName="1.15.2"
         * [Address(RVA = "0x1034DD0", Offset = "0x1034DD0", VA = "0x1034DD0")]
         *
         * android:versionCode="114100153" android:versionName="1.14.10"
         * [Address(RVA = "0x1013880", Offset = "0x1013880", VA = "0x1013880")]
         *
         * android:versionCode="114080151" android:versionName="1.14.8"
         * [Address(RVA = "0x1011810", Offset = "0x1011810", VA = "0x1011810")]
         *
         * android:versionCode="112060129" android:versionName="1.12.6"
         * [Address(RVA = "0xFEEF48", Offset = "0xFEEF48", VA = "0xFEEF48")]
         *
         * public static string string.Concat(string str0, string str1)
         */
        uintptr_t String_Concat;
    }

    void Initialize() {
        Methods::QuestionAnswerButton_Init = findPattern(
                "F0 4D 2D E9 18 B0 8D E2 70 41 9F E5 00 80 A0 E1 03 60 A0 E1 02 50 A0 E1 04 40 8F E0 01 70 A0 E1 00 00 D4 E5 00 00 50 E3 05 00 00 1A 50 01 9F E5 00 00 9F E7 00 00 90 E5 ? ? ? EB 01 00 A0 E3 00 00 C4 E5 08 00 A0 E1 51 00 00 EB 08 40 A0 E1 06 10 A0 E1 60 60 A4 E5 ");
        Methods::QuestionContainerClassic_GetTimerDuration = findPattern(
                "00 00 00 E3 A0 01 44 E3 1E FF 2F E1");
        Methods::VIPManager_HasVIPProperty = findPattern(
                "00 40 E2 01 00 50 E3 ? ? ? ? 00 48 2D E9 0D B0 A0 E1 ? ? ? ? 00 00 50 E3 00 48 BD E8 01 00 A0 13 1E FF 2F 11") -
                                             0x1;

        Methods::String_CreateString = findPattern(
                "30 48 2D E9 08 B0 8D E2 08 D0 4D E2 90 50 9F E5 01 40 A0 E1 05 50 8F E0 00 00 D5 E5 00 00 50 E3 05 00 00 1A 7C 00 9F E5 00 00 9F E7 00 00 90 E5 ? ? ? EB 01 00 A0 E3 00 00 C5 E5 00 00 54 E3 10 00 00 0A 00 10 D4 E5 00 00 A0 E3 00 30 A0 E3 00 00 51 E3 05 00 00 0A 00 30 A0 E3 04 10 A0 E1");
        Methods::String_Concat = findPattern(
                "F0 48 2D E9 10 B0 8D E2 E0 60 9F E5 00 50 A0 E1 01 40 A0 E1 06 60 8F E0 00 00 D6 E5 00 00 50 E3 05 00 00 1A C8 00 9F E5 00 00 9F E7 00 00 90 E5 ? ? ? EB 01 00 A0 E3 00 00 C6 E5 00 00 55 E3 01 10 A0 E3 08 10 95 15 01 00 A0 E3 11 1F 6F 11 A1 12 A0 11 00 00 54 E3 08 00 94 15 10 0F 6F 11");
    }
}

#endif //OFFSETS_H