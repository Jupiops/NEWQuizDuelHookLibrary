#pragma once

#include <android/log.h>
#include <string>
#include <cstdarg>

namespace hooklib {

    enum class LogLevel {
        Debug, Info, Warn, Error
    };

#ifndef HOOKLIB_LOG_TAG
#define HOOKLIB_LOG_TAG "HOOKLIB"
#endif

    class Logger {
    public:
        static void log(LogLevel lvl, const char *fmt, ...) __attribute__((format(printf, 2, 3)));

        static void setMinLevel(LogLevel lvl) noexcept { minLevel() = lvl; }

    private:
        static LogLevel &minLevel() {
            static LogLevel level = LogLevel::Debug;
            return level;
        }

        static int androidPriority(LogLevel lvl) {
            switch (lvl) {
                case LogLevel::Debug:
                    return ANDROID_LOG_DEBUG;
                case LogLevel::Info:
                    return ANDROID_LOG_INFO;
                case LogLevel::Warn:
                    return ANDROID_LOG_WARN;
                case LogLevel::Error:
                    return ANDROID_LOG_ERROR;
            }
            return ANDROID_LOG_DEBUG;
        }
    };

#define LOGD(fmt, ...) ::hooklib::Logger::log(::hooklib::LogLevel::Debug, "[D] " fmt, ##__VA_ARGS__)
#define LOGI(fmt, ...) ::hooklib::Logger::log(::hooklib::LogLevel::Info,  "[I] " fmt, ##__VA_ARGS__)
#define LOGW(fmt, ...) ::hooklib::Logger::log(::hooklib::LogLevel::Warn,  "[W] " fmt, ##__VA_ARGS__)
#define LOGE(fmt, ...) ::hooklib::Logger::log(::hooklib::LogLevel::Error, "[E] " fmt, ##__VA_ARGS__)

} // namespace hooklib