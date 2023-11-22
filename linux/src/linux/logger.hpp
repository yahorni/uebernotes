#pragma once

// #define ELPP_NO_DEFAULT_LOG_FILE
#include <easylogging++.h>

#include <format>
#include <string>
#include <utility>

#define LOGGER_STATIC_INIT INITIALIZE_EASYLOGGINGPP

namespace linux {

class Log {
private:
    static constexpr char _linuxLoggerName[]{"linux"};
    static constexpr char _coreLoggerName[]{"core"};

public:
    static void init(int argc, const char** argv);

#define LOG_LINUX(NAME, LEVEL)                                                                                         \
    template <typename... Args>                                                                                        \
    static constexpr void NAME(std::format_string<Args...> fmt_str, Args&&... args) {                                  \
        std::string msg;                                                                                               \
        auto it = std::back_inserter(msg);                                                                             \
        std::format_to(it, fmt_str, std::forward<Args>(args)...);                                                      \
        CLOG(LEVEL, _linuxLoggerName) << msg;                                                                          \
    }

    LOG_LINUX(trace, TRACE)
    LOG_LINUX(debug, DEBUG)
    LOG_LINUX(info, INFO)
    LOG_LINUX(warning, WARNING)
    LOG_LINUX(error, ERROR)
    LOG_LINUX(fatal, FATAL)

    static void coreTrace(const char* msg);
    static void coreDebug(const char* msg);
    static void coreInfo(const char* msg);
    static void coreWarning(const char* msg);
    static void coreError(const char* msg);
    static void coreFatal(const char* msg);
};

}  // namespace linux
