# Application-wide NDK settings

# Build both 32-bit and 64-bit ABIs
APP_ABI := armeabi-v7a arm64-v8a

# Optimize for release by default (use "debug" locally if needed)
APP_OPTIM := release

# Minimum platform level (21+ required for arm64)
APP_PLATFORM := android-23

# Use modern C++ STL in r16b+ (prefer c++_static to avoid dependency issues)
APP_STL := c++_static

# Smaller archives
APP_THIN_ARCHIVE := true

# Position Independent Executable/Code
APP_PIE := true


# Extra flags for non-debug builds
ifneq ($(APP_OPTIM),debug)
  $(info APP_OPTIM is $(APP_OPTIM) ...)
  APP_LDFLAGS += -Wl,--strip-all
  APP_CFLAGS  += -g0 -O3 -fomit-frame-pointer -ffunction-sections -fdata-sections
endif