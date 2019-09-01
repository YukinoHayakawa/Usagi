#pragma once

#include "Exception.hpp"

#define FMT_THROW(e) USAGI_THROW(e)
#include <fmt/format.h>
// enables outputting custom types
#include <fmt/ostream.h>

#define USAGI_FMT_USE_STREAM_OP(type) \
template <typename Char> \
struct fmt::formatter<type, Char> : \
    fmt::v6::internal::fallback_formatter<type, Char> { }; \
/**/
