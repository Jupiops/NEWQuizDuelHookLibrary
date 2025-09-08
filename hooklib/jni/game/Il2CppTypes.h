#pragma once

#include <cstdint>
#include <cstddef>

namespace il2cpp {

    struct ObjectBase {
        union {
            __unused void *klass;
            __unused void *vtable;
        };
        __unused void *monitor;
    };

    struct String {
        ObjectBase object;
        int32_t length;                             ///< Length of string *excluding* the trailing null (which is included in 'chars').
        char16_t data[1];

        using CreateFn = String *(*)(void *thisPtr, const char *str, int start, int len);
        using ConcatFn = String *(*)(String *, String *);
    };

} // namespace il2cpp