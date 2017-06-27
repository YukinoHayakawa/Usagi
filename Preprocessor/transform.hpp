#pragma once

#include "unpack.hpp"

#define YUKI_ID(r, data, x) x
#define YUKI_TUPLE_TO_PARAM_LIST(r, data, tuple) YUKI_UNPACK tuple
