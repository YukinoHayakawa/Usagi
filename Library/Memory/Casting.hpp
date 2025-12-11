#pragma once

#include <type_traits>

namespace usagi
{
template <typename To, typename From>
    requires (
        !std::is_reference_v<From> && !std::is_reference_v<To> &&
        sizeof(From) <= sizeof(void *) && sizeof(To) <= sizeof(void *)
    )
To force_cast_small_value(From && val)
{
    union c
    {
        From f;
        To   t;

        c(From && v)
            : f(std::forward<From>(v))
        {
        }
    };

    union
    {
        From * fp;
        To *   tp;
    };

    static_assert(sizeof(c) == sizeof(To));

    c conv { std::forward<From>(val) };
    fp = &conv.f;
    return *tp;
}
} // namespace usagi
