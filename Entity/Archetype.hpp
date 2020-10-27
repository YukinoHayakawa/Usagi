#pragma once

#include <tuple>
#include <cstdint>

#include "detail/ComponentFilter.hpp"

namespace usagi
{
template <Component... InitialComponents>
class Archetype
{
    std::tuple<InitialComponents...> mInitialValues;

    template <
        template <typename T> typename Storage,
        Component... EnabledComponents
    >
    friend class EntityDatabase;

    // These two fields combinedly determine that whether the last used page
    // can be reused. Beware that the the page index must refer to the same
    // entity database.
    std::uint64_t mLastUsedPageSeqId = -1;
    std::uint64_t mLastUsedPageIndex = -1;

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
