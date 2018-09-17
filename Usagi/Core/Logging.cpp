#include "Logging.hpp"

#include <spdlog/sinks/stdout_color_sinks.h>

namespace usagi
{
void doLog(const LoggingLevel level, const char *str)
{
    static auto logger = spdlog::stdout_color_mt("console");
    logger->log(static_cast<spdlog::level::level_enum>(level), str);
}
}
