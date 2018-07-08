#pragma once

#include "unpack.hpp"

#define USAGI_ID(r, data, x) x
#define USAGI_TUPLE_TO_PARAM_LIST(r, data, tuple) USAGI_UNPACK tuple
