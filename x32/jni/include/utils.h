#ifndef UTILS_H
#define UTILS_H

#include <jni.h>
#include <unistd.h>

#define INRANGE(x, a, b)    (x >= a && x <= b)
#define getBits(x)    (INRANGE((x&(~0x20)),'A','F') ? ((x&(~0x20)) - 'A' + 0xa) : (INRANGE(x,'0','9') ? x - '0' : 0))
#define getByte(x)    (getBits(x[0]) << 4 | getBits(x[1]))

typedef unsigned char BYTE, *PBYTE;
typedef unsigned short WORD, *PWORD;
typedef unsigned long DWORD;
typedef unsigned long long DWORD64;

DWORD libBase = 0;
DWORD libEnd = 0;

/**************************************
	ENTER THE GAME's LIB NAME HERE!
***************************************/
const char *libName = "libil2cpp.so";

DWORD get_libBase(const char *libName);

DWORD getRealOffset(DWORD address);

DWORD get_libBase(const char *libName) {
    FILE *fp;
    DWORD addr = 0;
    char filename[32], buffer[1024];
    snprintf(filename, sizeof(filename), "/proc/%d/maps", getpid());
    fp = fopen(filename, "rt");
    if (fp != NULL) {
        while (fgets(buffer, sizeof(buffer), fp)) {
            if (strstr(buffer, libName)) {
                addr = (uintptr_t) strtoul(buffer, NULL, 16);
                break;
            }
        }
        fclose(fp);
    }
    return addr;
}

DWORD getRealOffset(DWORD address) {
    if (libBase == 0) {
        libBase = get_libBase(libName);
    }
    return (libBase + address);
}

DWORD64 calcRelativeOffset(DWORD64 address) {
    if (address < libBase)
        address += libBase;

    return *(int32_t *) address + (address + 4) - libBase;
}

DWORD64 findPattern(const char *pattern) {
    const char *pat = pattern;
    DWORD64 firstMatch = 0;
    DWORD64 rangeStart = libBase;
    DWORD64 rangeEnd = libEnd;

    for (DWORD64 pCur = rangeStart; pCur < rangeEnd; pCur++) {
        if (!*pat)
            return firstMatch - libBase;

        if (*(PBYTE) pat == '\?' || *(BYTE *) pCur == getByte(pat)) {
            if (!firstMatch)
                firstMatch = pCur;

            if (!pat[2]) {
                if (pattern[0] == 'E' && (pattern[1] == '8' || pattern[1] == '9'))
                    return calcRelativeOffset(firstMatch + 1);
                return firstMatch - libBase;
            }

            if (*(PWORD) pat == '\?\?' || *(PBYTE) pat != '\?')
                pat += 3;
            else
                pat += 2;
        } else {
            pat = pattern;
            firstMatch = 0;
        }
    }
    return 0;
}

template<class T>
T *findFunction(DWORD offset) {
    if (libBase == 0) {
        libBase = get_libBase(libName);
    }
    return (T *) (libBase + offset);
}

#endif
