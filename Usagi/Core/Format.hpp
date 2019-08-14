#pragma once

#include "Exception.hpp"

#define FMT_THROW(e) USAGI_THROW(e)
#include <fmt/format.h>
// enables outputting custom types
#include <fmt/ostream.h>
