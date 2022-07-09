#pragma once

#include <algorithm>

namespace usagi
{
// credits: https://ctrpeach.io/posts/cpp20-string-literal-template-parameters/

template <std::size_t N>
struct StringLiteral
{
    char value[N] { };

    constexpr StringLiteral(const char (&str)[N])
    {
        std::copy_n(str, N, value);
    }
};
}
