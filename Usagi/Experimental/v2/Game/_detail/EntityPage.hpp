#pragma once

#include <array>

#include <Usagi/Experimental/v2/Game/Entity/Component.hpp>

#include "ComponentMask.hpp"

namespace usagi
{
template <
    std::uint16_t   NumEntities,
    Component...    EnabledComponents
>
struct EntityPage
{
    using ComponentMaskT = ComponentMask<EnabledComponents...>;
    constexpr static std::size_t INVALID_PAGE = -1;

    struct ComponentState
    {
        // Bitmask of created components of an entity
        ComponentMaskT owned;
    };

    template <Component C>
    struct PageIndex
    {
        std::size_t index = INVALID_PAGE;
    };

    template <Component C>
    std::size_t & componentPageIndex()
    {
        return std::get<PageIndex<C>>(component_page_indices).index;
    }

    // Specify the range of entity id represented by this page
    std::uint64_t first_entity_id = -1;
    std::uint16_t first_unused_index = 0;

    std::array<ComponentState, NumEntities> entity_states;

    // Bitwise-OR of component masks of all entities
    // ComponentMaskT  page_component_mask;

    // index into the page pool of each component.
    // -1 if the page is not allocated
    std::tuple<PageIndex<EnabledComponents>...> component_page_indices;
};
}
