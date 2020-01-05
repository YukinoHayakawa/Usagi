#pragma once

#include "Component.hpp"

namespace usagi::ecs
{
template <Component... InitialComponents>
class Archetype
{
    std::tuple<InitialComponents...> mInitialValues;

public:
    Archetype() = default;

    template <typename... Args>
    Archetype(Args &&...args)
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
