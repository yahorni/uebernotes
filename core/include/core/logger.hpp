#pragma once

#include <format>
#include <string>
#include <utility>

namespace core {

class Log {
    using CallbackType = void (*)(const char*);
    static void _suppressLogs(const char*) {}

#define LOG_CALLBACK(NAME, LEVEL)                                                                                      \
private:                                                                                                               \
    static CallbackType _log##NAME;                                                                                    \
                                                                                                                       \
public:                                                                                                                \
    static void set##NAME##Logger(CallbackType logger) { _log##NAME = logger; }                                        \
    template <typename... Args>                                                                                        \
    static constexpr void LEVEL(std::format_string<Args...> fmt_str, Args&&... args) {                                 \
        std::string msg;                                                                                               \
        auto it = std::back_inserter(msg);                                                                             \
        std::format_to(it, fmt_str, std::forward<Args>(args)...);                                                      \
        (*_log##NAME)(msg.c_str());                                                                                    \
    }

    LOG_CALLBACK(Trace, trace)
    LOG_CALLBACK(Debug, debug)
    LOG_CALLBACK(Info, info)
    LOG_CALLBACK(Warning, warning)
    LOG_CALLBACK(Error, error)
    LOG_CALLBACK(Fatal, fatal)
};

}  // namespace core
