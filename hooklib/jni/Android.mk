# Root Android.mk (placed next to jni/)
LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := hook
LOCAL_MODULE_FILENAME := libhook

# Common compiler flags
LOCAL_CPPFLAGS += -std=c++20 -fno-exceptions -fno-rtti -fvisibility=hidden -fvisibility-inlines-hidden
LOCAL_CPPFLAGS += -Wno-error=format-security -Wall -Wextra
LOCAL_CFLAGS   += -fvisibility=hidden -fvisibility-inlines-hidden -Wno-absolute-value

# Include paths
LOCAL_C_INCLUDES += \
    $(LOCAL_PATH) \
    $(LOCAL_PATH)/core \
    $(LOCAL_PATH)/game \
    $(LOCAL_PATH)/arch \
    $(LOCAL_PATH)/libs \
    $(LOCAL_PATH)/libs/And64InlineHook \
    $(LOCAL_PATH)/libs/Substrate

# ---- List sources explicitly (adjust to match your tree) ----
# Core & game (add the actual .cpp files that define Logger, MemoryMap, OffsetRegistry, targets, etc.)
LOCAL_SRC_FILES := \
  core/HookManager.cpp \
  core/Logger.cpp \
  core/MemoryMap.cpp \
  core/OffsetRegistry.cpp \
  core/PatternScanner.cpp \
  game/OffsetInit.cpp \
  game/Targets.cpp \
  main.cpp

# Arch-specific backends
ifeq ($(TARGET_ARCH_ABI),arm64-v8a)
  LOCAL_SRC_FILES += \
    arch/arm64/HookBackend.cpp \
    libs/And64InlineHook/And64InlineHook.cpp
else ifeq ($(TARGET_ARCH_ABI),armeabi-v7a)
  LOCAL_SRC_FILES += \
    arch/armv7/HookBackend.cpp \
    libs/Substrate/hde64.c \
    libs/Substrate/SubstrateDebug.cpp \
    libs/Substrate/SubstrateHook.cpp \
    libs/Substrate/SubstratePosixMemory.cpp
endif

# If you use KittyMemory (add when needed)
# LOCAL_SRC_FILES += \
#     libs/KittyMemory/KittyMemory.cpp \
#     libs/KittyMemory/MemoryPatch.cpp

# Linker flags
LOCAL_LDLIBS := -llog

# Debug: uncomment to see what is compiled
# $(info LOCAL_SRC_FILES=$(LOCAL_SRC_FILES))

include $(BUILD_SHARED_LIBRARY)