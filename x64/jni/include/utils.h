#ifndef UTILS_H
#define UTILS_H

#include <jni.h>
#include <android/log.h>
#include <unistd.h>
#include <cstdio>
#include <cstring>
#include <cstdint>
#include <limits>
#include <ctype.h>

#define LOG_TAG  "HOOK_LIBRARY"

#define  LOGE(...)  __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)
#define  LOGW(...)  __android_log_print(ANDROID_LOG_WARN,LOG_TAG,__VA_ARGS__)
#define  LOGD(...)  __android_log_print(ANDROID_LOG_DEBUG,LOG_TAG,__VA_ARGS__)
//#define  LOGI(...)  __android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__)

typedef unsigned char BYTE, *PBYTE;

uintptr_t libBase = 0;
uintptr_t libEnd = 0;

/**************************************
	ENTER THE GAME's LIB NAME HERE!
***************************************/
const char *libraryName = "libil2cpp.so";

uintptr_t getLibraryBase(const char *libraryName);

uintptr_t getRealOffset(uintptr_t address);

uintptr_t getLibraryBase(const char *libraryName) {
    char line[512] = {0};
    uintptr_t startAddr, endAddr;
    bool found = false;

    // Initialize libBase to the maximum possible value and libEnd to 0
    libBase = std::numeric_limits<uintptr_t>::max();
    libEnd = 0;

    FILE *fp = fopen("/proc/self/maps", "rt");
    if (fp != NULL) {
        while (fgets(line, sizeof(line), fp)) {
            if (strstr(line, libraryName)) {
                // Parse the line to get start and end addresses
                sscanf(line, "%lx-%lx", &startAddr, &endAddr);

                // Update libBase to the smallest start address
                if (startAddr < libBase) {
                    libBase = startAddr;
                }

                // Update libEnd to the largest end address
                if (endAddr > libEnd) {
                    libEnd = endAddr;
                }

                found = true; // At least one match was found
            }
        }
        fclose(fp);
    }

    // If no match was found, return false
    return found;
}

uintptr_t getRealOffset(uintptr_t address) {
    if (libBase == 0) {
        libBase = getLibraryBase(libraryName);
    }
    return (libBase + address);
}

uintptr_t calcRelativeOffset(uintptr_t address) {
    if (address < libBase)
        address += libBase;

    // Check that the address is within the valid range
    if (address < libBase || address >= libEnd) {
        LOGE("calcRelativeOffset: Address out of bounds!");
        return 0;
    }

    // Ensure the offset calculation is properly done for 64-bit architectures.
    return *(int32_t *) address + (address + 4) - libBase;
}

int countPatternBytes(const char *pattern) {
    int count = 0;
    while (*pattern != '\0') {
        // Skip any spaces
        if (*pattern == ' ') {
            pattern++;
            continue;
        }

        // Check if we encounter a '?', count it as a single byte
        if (*pattern == '?') {
            count++;
            pattern++;
            // Check for a double '?', move the pointer accordingly
            if (*pattern == '?') {
                pattern++;
            }
        } else if (isxdigit(pattern[0]) && isxdigit(pattern[1])) {
            // If it's a valid hex byte (two hex digits), count it as one byte
            count++;
            pattern += 2; // Move past the two hex digits
        }

        // Skip spaces between bytes (if any)
        while (*pattern == ' ') {
            pattern++;
        }
    }
    return count;
}

uintptr_t find_pattern(const char *pattern) {
    uint8_t *baseAddr = (uint8_t *) libBase;
    uint8_t *endAddr = (uint8_t *) libEnd;
    int patternLength = countPatternBytes(pattern); // Cache the length of the pattern
    LOGD("Starting pattern search in range: 0x%lx - 0x%lx, pattern length: %d bytes\n", libBase,
         libEnd, patternLength);

    // Precompute the hex byte pattern and wildcard positions
    uint8_t patternBytes[patternLength];
    bool wildcard[patternLength];
    int byteIdx = 0;

    while (*pattern) {
        if (*pattern == ' ') {
            pattern++;  // Skip spaces
            continue;
        }
        if (byteIdx >= patternLength) {
            LOGE("Pattern longer than expected");
            return 0;
        }

        if (*pattern == '?') {
            wildcard[byteIdx] = true; // Mark as wildcard
            pattern++;
            if (*pattern == '?') pattern++;
            byteIdx++;
        } else if (isxdigit(pattern[0]) && isxdigit(pattern[1])) {
            // Convert hex string to byte using bitwise operations
            uint8_t byte =
                    ((pattern[0] > '9' ? (pattern[0] & ~0x20) - 'A' + 10 : pattern[0] - '0') << 4)
                    | (pattern[1] > '9' ? (pattern[1] & ~0x20) - 'A' + 10 : pattern[1] - '0');

            patternBytes[byteIdx] = byte;
            wildcard[byteIdx] = false; // Mark as not wildcard
            byteIdx++;
            pattern += 2;
        } else {
            LOGE("Invalid pattern format");
            return 0;
        }
    }

    // Iterate over the memory range and match the precomputed pattern
    for (uint8_t *addr = baseAddr; addr <= endAddr - patternLength; addr++) {
        bool match = true;
        for (int i = 0; i < patternLength; i++) {
            if (!wildcard[i] && addr[i] != patternBytes[i]) {
                match = false;
                break;
            }
        }
        if (match) {
            LOGD("Final match found at address: %p\n", addr);
            return (uintptr_t)(addr - baseAddr);  // Return the relative offset
        }
    }

    LOGD("Pattern not found in the specified range.\n");
    return 0;  // Pattern not found
}

// Function to log the bytes at a specific relative offset
void logMemoryAtOffset(uintptr_t relativeOffset, size_t length) {
    if (libBase == 0) {
        LOGE("logMemoryAtOffset: Library base not initialized!");
        return;
    }

    uintptr_t address = getRealOffset(relativeOffset);  // Calculate the absolute address

    LOGD("Memory dump at offset: %lx (absolute address: %lx)", relativeOffset, address);

    // Read and print the memory at the calculated address
    PBYTE memory = (PBYTE) address;
    for (size_t i = 0; i < length; i++) {
        LOGD("0x%02X ", memory[i]);  // Log each byte as hex
    }
}

template<class T>
T *findFunction(uintptr_t offset) {
    if (libBase == 0) {
        libBase = getLibraryBase(libraryName);
    }
    return (T *) (libBase + offset);
}

#endif
