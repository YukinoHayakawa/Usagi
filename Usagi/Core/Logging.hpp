#pragma once

#include <fmt/format.h>
// enables outputting custom types
#include <fmt/ostream.h>

namespace usagi
{
enum class LoggingLevel
{
    trace = 0,
    debug = 1,
    info = 2,
    warn = 3,
    error = 4,
    critical = 5,
    off = 6
};

bool shouldLog(LoggingLevel level);
void doLog(LoggingLevel level, const char *str);

template <typename... Args>
void log(const LoggingLevel level, const char *fmt, Args &&... args)
{
    if(!shouldLog(level)) return;

    fmt::format_arg_store<fmt::format_context, Args...> as {
        std::forward<Args>(args)...
    };
    fmt::memory_buffer buffer;
    fmt::vformat_to(buffer, fmt, as);
    doLog(level, buffer.data());
}

template <typename T>
void log(const LoggingLevel level, const T &arg)
{
    log(level, "{}", arg);
}
}

#define LOG(level, ...) ::usagi::log(::usagi::LoggingLevel::level, __VA_ARGS__)
