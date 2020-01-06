#include "Logging.hpp"

#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/basic_file_sink.h>

namespace
{
using namespace spdlog;
using namespace spdlog::details;

// print elapsed time since logger creation
class logger_elapsed_formatter final : public formatter
{
public:
    logger_elapsed_formatter()
        : logger_creation_time_(log_clock::now())
    {
    }

    void format(const details::log_msg &msg, memory_buf_t &dest) override
    {
        using namespace std::chrono;
        const duration<double> delta = msg.time - logger_creation_time_;
        fmt::format_to(dest, "[{:>15.6f}] ", delta.count());
    }

    std::unique_ptr<formatter> clone() const override
    {
        return std::make_unique<logger_elapsed_formatter>(*this);
    }

protected:
    const log_clock::time_point logger_creation_time_;
};

class DefaultFormatter : public formatter
{
    logger_elapsed_formatter mTime;

public:
    void format(const details::log_msg &msg, memory_buf_t &dest) override
    {
        mTime.format(msg, dest);

        // following code copied from spdlog/details/pattern_formatter-inl.h

// #ifndef SPDLOG_NO_NAME
//         if(msg.logger_name.size() > 0)
//         {
//             dest.push_back('[');
//             // fmt_helper::append_str(*msg.logger_name, dest);
//             fmt_helper::append_string_view(msg.logger_name, dest);
//             dest.push_back(']');
//             dest.push_back(' ');
//         }
// #endif
        dest.push_back('[');
        // wrap the level name with color
        msg.color_range_start = dest.size();
        // fmt_helper::append_string_view(level::to_c_str(msg.level), dest);
        fmt_helper::append_string_view(level::to_string_view(msg.level), dest);
        msg.color_range_end = dest.size();
        dest.push_back(']');
        dest.push_back(' ');

        // add source location if present
        if(!msg.source.empty())
        {
            dest.push_back('[');
            const char *filename = details::short_filename_formatter<details::
                null_scoped_padder>::basename(msg.source.filename);
            fmt_helper::append_string_view(filename, dest);
            dest.push_back(':');
            fmt_helper::append_int(msg.source.line, dest);
            dest.push_back(']');
            dest.push_back(' ');
        }
        // fmt_helper::append_string_view(msg.msg(), dest);
        fmt_helper::append_string_view(msg.payload, dest);
        dest.push_back('\n');
    }

    std::unique_ptr<formatter> clone() const override
    {
        return std::make_unique<DefaultFormatter>(*this);
    }
};

auto gLogger = []() {
    using namespace spdlog;
    auto logger = stdout_color_mt("default");
    logger->set_formatter(std::make_unique<DefaultFormatter>());
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
