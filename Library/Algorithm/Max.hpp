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
            max_val = std::move(val);
        }
    }

    return std::make_tuple(std::move(max_iter), std::move(max_val));
}
}
