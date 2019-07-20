#pragma once

#include <fmt/format.h>
// enables outputting custom types
#include <fmt/ostream.h>

namespace usagi::logging
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
void doLog(LoggingLevel level, std::string_view msg);
void addFileSink(const std::string &file_path);

template <typename... Args>
void log(const LoggingLevel level, std::string_view fmt, Args &&... args)
{
    if(!shouldLog(level)) return;

    fmt::memory_buffer buffer;
    fmt::format_to(buffer, fmt, std::forward<Args>(args)...);
    doLog(level, { buffer.data(), buffer.size() });
}
}

#define LOG(level, ...) \
    ::usagi::logging::log(\
        ::usagi::logging::LoggingLevel::level, __VA_ARGS__) \
/**/
