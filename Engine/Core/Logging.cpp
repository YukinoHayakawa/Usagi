#include "Logging.hpp"

#include <spdlog/sinks/stdout_color_sinks.h>

namespace usagi
{
std::shared_ptr<spdlog::logger> gConsoleLogger =
    spdlog::stdout_color_mt("console");
}
