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
#define USAGI_ASSERT_THROW(p, exception) \
    do { \
        if(!(p)) throw exception; \
    } while(0) \
/**/
#define USAGI_UNREACHABLE(msg) throw ::usagi::ExceptionUnreachableCode(msg)
#define USAGI_INVALID_ENUM_VALUE() USAGI_UNREACHABLE("Invalid enum value.")
