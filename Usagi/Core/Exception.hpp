#pragma once

#include <boost/stacktrace/stacktrace.hpp>
#include <boost/exception/info.hpp>
#include <boost/exception/enable_error_info.hpp>
#include <boost/exception/get_error_info.hpp>

namespace usagi
{
using ExceptionStackTrace = boost::error_info<
    struct tag_stacktrace,
    boost::stacktrace::stacktrace
>;

ExceptionStackTrace getCurrentStackTrace();

template <class E>
[[noreturn]] void throwWithStackTrace(const E &e)
{
    throw boost::enable_error_info(e) << getCurrentStackTrace();
}

template <class E>
auto getStackTrace(const E &e)
{
    return boost::get_error_info<ExceptionStackTrace>(e);
}
}

#define USAGI_THROW(e) ::usagi::throwWithStackTrace(e)
