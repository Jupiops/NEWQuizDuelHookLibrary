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
         * private unsafe string string.CreateString(sbyte* value)
         */
        DWORD String_Create;
        /*
         * public static string string.Concat(string str0, string str1)
         */
        DWORD String_Concat;
    }

    void Initialize() {
        Methods::String_Create = 0x1020700;
        Methods::String_Concat = 0x1011810;
    }
}

#endif //H
