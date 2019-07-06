#include "Logging.hpp"

#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>

namespace
{
auto gLogger = []() {
    auto logger = spdlog::stdout_color_mt("default");
    // logger->set_pattern()
    return std::move(logger);
}();
}

namespace usagi
{
bool shouldLog(LoggingLevel level)
{
    return gLogger->should_log(static_cast<spdlog::level::level_enum>(level));
}

void doLog(const LoggingLevel level, std::string_view msg)
{
    gLogger->log(
        spdlog::source_loc { },
        static_cast<spdlog::level::level_enum>(level),
        msg
    );
}
}
