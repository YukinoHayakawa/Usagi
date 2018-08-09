#pragma once

#include <vector>
#include <algorithm>
#include <type_traits>

namespace usagi::vulkan
{
template <
    typename SrcContainer,
    typename TransFunc,
    typename DestT =
        std::invoke_result_t<TransFunc, typename SrcContainer::value_type>
>
auto transformObjects(SrcContainer &src, TransFunc func)
{
    std::vector<DestT> dest;
    dest.reserve(src.size());
    std::transform(src.begin(), src.end(), std::back_inserter(dest), func);
    return dest;
}
}
