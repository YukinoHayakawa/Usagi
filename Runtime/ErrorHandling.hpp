#pragma once

#include <stdexcept>

namespace usagi
{
class ExceptionUnreachableCode : public std::logic_error
{
public:
    using logic_error::logic_error;
};
}

// todo stacktrace
#define USAGI_THROW(exception) throw exception
#define USAGI_UNREACHABLE() throw ::usagi::ExceptionUnreachableCode("")
