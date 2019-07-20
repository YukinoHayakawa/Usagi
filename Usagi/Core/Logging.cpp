#include "Logging.hpp"

#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/basic_file_sink.h>

namespace
{
auto gLogger = []() {
    using namespace spdlog;
    auto logger = stdout_color_mt("default");
    // logger->set_pattern()
    logger->set_level(level::level_enum::trace);
    return std::move(logger);
}();
}

namespace usagi::logging
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

void addFileSink(const std::string &file_path)
{
    gLogger->sinks().push_back(
        std::make_shared<spdlog::sinks::basic_file_sink_mt>(
            file_path
        ));
}
}
