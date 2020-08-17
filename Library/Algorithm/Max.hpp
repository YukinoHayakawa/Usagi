#pragma once

#include <cassert>

namespace usagi
{
template <typename Iter, typename Eval>
auto max_value(Iter begin, Iter end, Eval eval)
{
    assert(begin != end);

    auto max = eval(*begin);
    ++begin;
    while(begin != end)
    {
        auto val = eval(*begin);
        if(val > max) max = val;
        ++begin;
    }

    return max;
}
}
