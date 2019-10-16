#pragma once

#include <boost/stacktrace/stacktrace.hpp>
#include <boost/exception/info.hpp>
#include <boost/exception/enable_error_info.hpp>
#include <boost/exception/get_error_info.hpp>

namespace usagi::exception
{
using ExceptionStacktrace = boost::error_info<
    struct tag_stacktrace,
    boost::stacktrace::stacktrace
>;

ExceptionStacktrace getExceptionStacktrace();

template <class E>
[[noreturn]] void throwWithStacktrace(const E &e)
{
    throw boost::enable_error_info(e) << getExceptionStacktrace();
}

template <class E>
auto getStacktrace(const E &e)
{
    return boost::get_error_info<ExceptionStacktrace>(e);
}

void printStacktrace();
}

#define USAGI_THROW(e) ::usagi::exception::throwWithStacktrace(e)
