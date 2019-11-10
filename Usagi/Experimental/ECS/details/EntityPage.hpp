#pragma once

#include <array>

#include "../Component.hpp"

namespace usagi
{
template <
    std::size_t NumEntities,
    Component... EnabledComponents
>
struct alignas(64) EntityPage
{
    constexpr static std::size_t INVALID_PAGE = -1;
    using ComponentMaskT = ComponentMask<EnabledComponents...>;

    // Entity Component Masks

    struct ComponentState
    {
        // Bit mask of created components of an entity
        ComponentMaskT owned;
    };

    std::array<ComponentState, NumEntities> entity_states;

    // Component Storage References

    template <Component C>
    struct PageIndex
    {
        std::size_t index = INVALID_PAGE;
    };

    // index into the page pool of each component.
    // -1 if the page is not allocated
    std::tuple<PageIndex<EnabledComponents>...> component_page_indices;

    // Assistant Accessors

    template <Component C>
    std::size_t & componentPageIndex()
    {
        return std::get<PageIndex<C>>(component_page_indices);
    }
};
}
