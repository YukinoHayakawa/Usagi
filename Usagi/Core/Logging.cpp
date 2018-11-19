#include "Logging.hpp"

#include <spdlog/sinks/stdout_color_sinks.h>

namespace
{
auto gLogger = spdlog::stdout_color_mt("console");
}

namespace usagi
{
bool shouldLog(LoggingLevel level)
{
    return gLogger->should_log(static_cast<spdlog::level::level_enum>(level));
}

void doLog(const LoggingLevel level, const char *str)
{
    gLogger->log(static_cast<spdlog::level::level_enum>(level), str);
}
}
