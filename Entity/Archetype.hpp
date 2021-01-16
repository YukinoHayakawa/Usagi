#pragma once

#include <cstdint>

#include "detail/ComponentFilter.hpp"

namespace usagi
{
template <Component... InitialComponents>
class Archetype : InitialComponents...
{
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

    template <Component... Cs>
    using Derived = Archetype<InitialComponents..., Cs...>;

    explicit Archetype(InitialComponents &&... args) requires
        (sizeof...(InitialComponents) > 0)
        : InitialComponents { std::forward<InitialComponents>(args) }...
    {
    }

    template <Component C>
    C & component()
    {
        return *this;
    }

    template <Component C>
    const C & component() const
    {
        return *this;
    }

    template <Component C>
    C & operator()(C)
    {
        return *this;
    }

    template <Component C>
    const C & operator()(C) const
    {
        return *this;
    }

    // Copy from existing entity view.
    template <typename EntityViewT>
    Archetype & operator=(EntityViewT &&view)
    {
        (..., (component<InitialComponents>() =
            view.component(Tag<InitialComponents>()
        )));
        return *this;
    }
};
}
