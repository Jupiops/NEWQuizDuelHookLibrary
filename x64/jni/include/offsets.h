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
         * [Address(RVA = "0xD7F400", Offset = "0xD7F400", VA = "0xD7F400")]
         *
         * android:versionCode="114080151" android:versionName="1.14.8"
         * [Address(RVA = "0xD6F8E0", Offset = "0xD6F8E0", VA = "0xD6F8E0")]
         *
         * android:versionCode="112060129" android:versionName="1.12.6"
         * [Address(RVA = "0xD1ADD8", Offset = "0xD1ADD8", VA = "0xD1ADD8")]
         *
         * public void QuizDuel.QuestionAnswerButton.Init(int answerIndex, string text, ButtonClick onClick)
         */
        DWORD QuestionAnswerButton_Init;
        /*
         * android:versionCode="115020157" android:versionName="1.15.2"
         * [Address(RVA = "0xE7AF68", Offset = "0xE7AF68", VA = "0xE7AF68", Slot = "25")]
         *
         * android:versionCode="114080151" android:versionName="1.14.8"
         * [Address(RVA = "0xD9C06C", Offset = "0xD9C06C", VA = "0xD9C06C", Slot = "25")]
         *
         * android:versionCode="112060129" android:versionName="1.12.6"
         * [Address(RVA = "0xDACAA0", Offset = "0xDACAA0", VA = "0xDACAA0", Slot = "25")]
         *
         * protected override float QuizDuel.Classic.QuestionContainerClassic.GetTimerDuration()
         */
        DWORD QuestionContainerClassic_GetTimerDuration;
        /*
         * android:versionCode="115020157" android:versionName="1.15.2"
         * [Address(RVA = "0xD3E708", Offset = "0xD3E708", VA = "0xD3E708")]
         *
         * android:versionCode="114080151" android:versionName="1.14.8"
         * [Address(RVA = "0xD1B010", Offset = "0xD1B010", VA = "0xD1B010")]
         *
         * android:versionCode="112060129" android:versionName="1.12.6"
         * [Address(RVA = "0xD0EA04", Offset = "0xD0EA04", VA = "0xD0EA04")]
         *
         * public static bool VIPManager.HasVIPProperty(VIPProperty property)
         */
        DWORD VIPManager_HasVIPProperty;
        /*
         * android:versionCode="115020157" android:versionName="1.15.2"
         * [Address(RVA = "0x15860D4", Offset = "0x15860D4", VA = "0x15860D4")]
         *
         * android:versionCode="114080151" android:versionName="1.14.8"
         * [Address(RVA = "0x1563C0C", Offset = "0x1563C0C", VA = "0x1563C0C")]
         *
         * android:versionCode="112060129" android:versionName="1.12.6"
         * [Address(RVA = "0x1549D74", Offset = "0x1549D74", VA = "0x1549D74")]
         *
         * private unsafe string string.CreateString(sbyte* value)
         */
        DWORD String_CreateString;
        /*
         * android:versionCode="115020157" android:versionName="1.15.2"
         * [Address(RVA = "0x1579B3C", Offset = "0x1579B3C", VA = "0x1579B3C")]
         *
         * android:versionCode="114080151" android:versionName="1.14.8"
         * [Address(RVA = "0x1557674", Offset = "0x1557674", VA = "0x1557674")]
         *
         * android:versionCode="112060129" android:versionName="1.12.6"
         * [Address(RVA = "0x153D7DC", Offset = "0x153D7DC", VA = "0x153D7DC")]
         *
         * public static string string.Concat(string str0, string str1)
         */
        DWORD String_Concat;
    }

    void Initialize() {
        Methods::QuestionAnswerButton_Init = findPattern(
                "F7 0F 1C F8 F6 57 01 A9 F4 4F 02 A9 FD 7B 03 A9 FD C3 00 91 ? FD 00 ? E8 ? ? 39 F5 03 03 AA F4 03 02 AA F6 03 01 2A F3 03 00 AA E8 00 00 37 ? EF 00 ? 08 ? ? F9 00 01 40 B9 ? ? ? 97 E8 03 00 32 E8 ? ? 39 E0 03 13 AA 3B 00 00 94 68 16 40 F9 76 AA 00 B9 75 4A 00 F9 C8 06 00 B4");
        Methods::QuestionContainerClassic_GetTimerDuration = findPattern("00 90 26 1E C0 03 5F D6");
        Methods::VIPManager_HasVIPProperty = findPattern(
                "08 04 00 51 1F 05 00 71 08 01 00 54 FD 7B BF A9 FD 03 00 91 06 00 00 94 FD 7B C1 A8 60 00 00 36 E0 03 00 32 C0 03 5F D6 28 00 00 14");

        Methods::String_CreateString = findPattern(
                "F4 4F BE A9 FD 7B 01 A9 FD 43 00 91 ? ? 00 ? 88 ? ? 39 F3 03 01 AA E8 00 00 37 ? ? 00 ? 08 ? ? F9 00 01 40 B9 ? ? ? 97 E8 03 00 32 88 ? ? 39 33 01 00 B4 68 02 40 39 E8 01 00 34 E3 03 1F 2A E8 03 13 AA 09 1D 40 38 63 04 00 11 C9 FF FF 35");
        Methods::String_Concat = findPattern(
                "F6 57 BD A9 F4 4F 01 A9 FD 7B 02 A9 FD 83 00 91 ? ? 00 ? A8 ? ? 39 F3 03 01 AA F4 03 00 AA E8 00 00 37 ? ? 00 B0 08 ? ? F9 00 01 40 B9 ? ? ? 97 E8 03 00 32 A8 ? ? 39 14 02 00 B4 88 12 40 B9 1F 01 00 71 E8 17 9F 1A D3 01 00 B4 69 12 40 B9");
    }
}

#endif //OFFSETS_H