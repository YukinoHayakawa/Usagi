#pragma once

#include <tuple>
#include <cassert>

namespace usagi
{
template <typename Iter, typename Eval>
auto max_eval(Iter begin, Iter end, Eval eval)
{
    assert(begin != end);

    Iter max_iter = begin;
    auto max_val = eval(*begin);
    while(true)
    {
        ++begin;
        if(begin == end) break;
        auto val = eval(*begin);
        if(max_val < val)
        {
            max_iter = begin;
            max_val = val;
        }
    }

    return std::make_tuple(max_iter, max_val);
}
}
