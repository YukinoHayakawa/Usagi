#include "Exception.hpp"

// note: remove the .lib built by vcpkg since they will cause linking conflict
// https://github.com/microsoft/vcpkg/issues/7678
#include <boost/stacktrace.hpp>

namespace usagi
{
ExceptionStackTrace getCurrentStackTrace()
{
    return ExceptionStackTrace(boost::stacktrace::stacktrace());
}
}
