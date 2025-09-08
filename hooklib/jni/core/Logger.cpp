#include "Logger.h"

namespace hooklib {

    void Logger::log(LogLevel lvl, const char *fmt, ...) {
        if (lvl < minLevel()) return;
        va_list args;
        va_start(args, fmt);
        __android_log_vprint(androidPriority(lvl), HOOKLIB_LOG_TAG, fmt, args);
        va_end(args);
    }

} // namespace hooklib