#pragma once

#include <stdexcept>

#define YUKI_DECL_RUNTIME_EXCEPTION(exception_name) \
class exception_name ## Exception : public std::runtime_error { \
public: \
    using std::runtime_error::runtime_error; \
} \
/**/
