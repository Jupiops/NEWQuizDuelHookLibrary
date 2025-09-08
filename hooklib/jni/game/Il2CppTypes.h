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

        using CreateFn = String *(*)(String *thisPtr, const char *str, int32_t start, int32_t len);
        using ConcatFn = String *(*)(String *, String *);
    };

    // -------------------------------
    // System.Nullable<T> (value-type)
    // -------------------------------
    template<typename T>
    struct Nullable {
        bool has_value; // IL2CPP layout: has_value first…
        T value;     // …then the underlying value (valid iff has_value == true)
    };

    // Convenience alias for System_Nullable_int__o
    using SystemNullableInt = Nullable<int32_t>;

    // Tiny helpers (header-only, inline)
    inline SystemNullableInt make_null() { return {false, 0}; }

    inline SystemNullableInt make_nullable(int32_t v) { return {true, v}; }

    inline bool is_null(const SystemNullableInt &n) { return !n.has_value; }

    // Sanity checks for the common ABI (bool at 0, int32 at offset 4)
    static_assert(offsetof(SystemNullableInt, has_value) == 0, "Nullable<int>: has_value offset");
    static_assert(offsetof(SystemNullableInt, value) == 4, "Nullable<int>: value offset");
    static_assert(sizeof(SystemNullableInt) == 8, "Nullable<int>: size");

} // namespace il2cpp