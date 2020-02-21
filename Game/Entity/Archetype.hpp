#pragma once

#include <tuple>

#include <Usagi/Game/_detail/ComponentFilter.hpp>

namespace usagi
{
template <Component... InitialComponents>
class Archetype
{
    std::tuple<InitialComponents...> mInitialValues;

    template <
        typename ComponentFilter,
        typename Storage
    >
    friend class EntityDatabase;

    std::size_t mLastUsedPageIndex = -1;

public:
    Archetype() = default;

    using ComponentFilterT = ComponentFilter<InitialComponents...>;

    template <typename... Args>
    explicit Archetype(Args &&... args)
        : mInitialValues { std::forward<Args>(args)... }
    {
    }

    template <Component C>
    auto & val()
    {
        return std::get<C>(mInitialValues);
    }

    template <Component C>
    auto & val() const
    {
        return std::get<C>(mInitialValues);
    }
};
}
