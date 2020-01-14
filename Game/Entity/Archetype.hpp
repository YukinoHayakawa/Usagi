#pragma once

#include <tuple>

#include <Usagi/Experimental/v2/Game/_detail/ComponentFilter.hpp>

#include "Component.hpp"

namespace usagi
{
template <Component... InitialComponents>
class Archetype
{
    std::tuple<InitialComponents...> mInitialValues;

public:
    Archetype() = default;

    using ComponentFilterT = ComponentFilter<InitialComponents...>;

    template <typename... Args>
    explicit Archetype(Args &&... args)
        : mInitialValues { std::forward<Args>(args)... }
    {
    }

    template <Component C>
    auto & initialValue()
    {
        return std::get<C>(mInitialValues);
    }

    template <Component C>
    auto & initialValue() const
    {
        return std::get<C>(mInitialValues);
    }
};
}
