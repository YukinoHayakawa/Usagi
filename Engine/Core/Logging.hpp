#pragma once

#include <spdlog/spdlog.h>
// enables outputting custom types
#include <fmt/ostream.h>

// fix namespace pollution
#ifdef _WIN32
#include <Usagi/Engine/Extension/Win32/Win32MacroFix.hpp>
#endif

namespace usagi
{
extern std::shared_ptr<spdlog::logger> gConsoleLogger;
}

#define LOG(level, ...) ::usagi::gConsoleLogger->level(__VA_ARGS__)
