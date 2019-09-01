#include "Exception.hpp"

// note: remove the .lib built by vcpkg since they will cause linking conflict
// https://github.com/microsoft/vcpkg/issues/7678
#include <boost/stacktrace.hpp>

#include "Logging.hpp"

namespace usagi::exception
{
ExceptionStacktrace getExceptionStacktrace()
{
    // skip first 3 frames in getting the stacktrace
    auto st = boost::stacktrace::stacktrace(3, -1);
    LOG(error, "Exception thrown!\nStacktrace: \n{}", st);
    return { std::move(st) };
}
}

#ifdef _MSC_VER
#   ifdef _DEBUG
#       pragma comment(lib, "boost_stacktrace_windbg_cached-vc140-mt-gd.lib")
#   else
#       pragma comment(lib, "boost_stacktrace_windbg_cached-vc140-mt.lib")
#   endif
#endif
