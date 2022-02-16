#pragma once

#include <tuple>

// Use tuple element values to invoke a function.
#define USAGI_APPLY(func, tuple) \
    std::apply( \
        [&]<typename... FwdArgs>(FwdArgs &&...fwd_args) -> decltype(auto) { \
            return func(std::forward<FwdArgs>(fwd_args)...); \
        }, tuple) \
/**/
