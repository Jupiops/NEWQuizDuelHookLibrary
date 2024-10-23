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
         * android:versionCode="141040468" android:versionName="1.41.04"
         * [Address(RVA = "0x1A79C44", Offset = "0x1A78C44", VA = "0x1A79C44")]
         *
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
        uintptr_t QuestionAnswerButton_Init;
        /*
         * android:versionCode="141040468" android:versionName="1.41.04"
         * [Address(RVA = "0x1B5658C", Offset = "0x1B5558C", VA = "0x1B5658C", Slot = "25")]
         *
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
        uintptr_t QuestionContainerClassic_GetTimerDuration;
        /*
         * android:versionCode="141040468" android:versionName="1.41.04"
         * [Address(RVA = "0x1AF6F50", Offset = "0x1AF5F50", VA = "0x1AF6F50")]
         *
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
        uintptr_t VIPManager_HasVIPProperty;
        /*
         * android:versionCode="141040468" android:versionName="1.41.04"
         * [Address(RVA = "0x2E506D4", Offset = "0x2E4F6D4", VA = "0x2E506D4")]
         *
         * android:versionCode="115020157" android:versionName="1.15.2"
         * [Address(RVA = "0x158643C", Offset = "0x158643C", VA = "0x158643C")]
         *
         * private unsafe string string.CreateString(sbyte* value, int startIndex, int length)
         *
         * android:versionCode="115020157" android:versionName="1.15.2"
         * [Address(RVA = "0x15860D4", Offset = "0x15860D4", VA = "0x15860D4")]
         *
         * android:versionCode="114080151" android:versionName="1.14.8"
         * [Address(RVA = "0x1563C0C", Offset = "0x1563C0C", VA = "0x1563C0C")]
         *
         * android:versionCode="112060129" android:versionName="1.12.6"
         * [Address(RVA = "0x1549D74", Offset = "0x1549D74", VA = "0x1549D74")]
         *
         * ~~private unsafe string string.CreateString(sbyte* value)~~
         */
        uintptr_t String_CreateString;
        /*
         * android:versionCode="141040468" android:versionName="1.41.04"
         * [Address(RVA = "0x2E3DFE4", Offset = "0x2E3CFE4", VA = "0x2E3DFE4")]
         *
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
        uintptr_t String_Concat;
    }

    void Initialize() {
        Methods::QuestionAnswerButton_Init = find_pattern("? ? ? F8 ? ? ? A9 ? ? ? A9 ? ? ? A9 ? ? ? D0 ? ? ? 39 F6 03 03 AA F5 03 02 AA F4 03 01 2A");
        Methods::QuestionContainerClassic_GetTimerDuration = find_pattern("? ? ? F8 ? ? ? A9 ? ? ? D0 ? ? ? B0 ? ? ? 39 ? ? ? F9 ? ? ? 37 ? ? ? B0 ? ? ? F9 ? ? ? 97 ? ? ? 52 ? ? ? 39 ? ? ? F9 ? ? ? B9 ? ? ? 35 ? ? ? 97 E0 03 1F AA ? ? ? 97");
        Methods::VIPManager_HasVIPProperty = find_pattern("? ? ? 71 ? ? ? 54 ? ? ? F8 ? ? ? 94 ? ? ? F8 ? ? ? 36");

        Methods::String_CreateString = find_pattern("E0 03 01 AA E1 03 02 2A E2 03 03 2A ? ? ? 17 ? ? ? 17");
        Methods::String_Concat = find_pattern("? ? ? F8 ? ? ? A9 ? ? ? A9 ? ? ? B0 ? ? ? 39 F4 03 01 AA F3 03 00 AA ? ? ? 37 ? ? ? F0 ? ? ? F9 ? ? ? 97 ? ? ? 52 ? ? ? 39 ? ? ? B4 ? ? ? B9");
    }
}

#endif //OFFSETS_H