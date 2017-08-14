#pragma once

#include <memory>

namespace yuki
{

template <typename Target, typename Source>
std::shared_ptr<Target> dynamic_pointer_cast_throw(const std::shared_ptr<Source> &source)
{
    auto result = std::dynamic_pointer_cast<Target>(source);
    if(!result) throw std::bad_cast();
    return result;
}

}
