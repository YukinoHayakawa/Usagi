#pragma once

#include <array>

#include "../Component.hpp"
#include "ComponentFilter.hpp"

namespace usagi::ecs
{
template <
    std::size_t NumEntities,
    Component... EnabledComponents
>
struct alignas(64) EntityPage
{
public:
    using ComponentMaskT = ComponentFilter<EnabledComponents...>;
    constexpr static std::size_t INVALID_PAGE = -1;

public:
    struct ComponentState
    {
        // Bit mask of created components of an entity
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
        return std::get<PageIndex<C>>(component_page_indices);
    }

public:
    std::size_t first_entity_id = 0;
    std::array<ComponentState, NumEntities> entity_states;

    // index into the page pool of each component.
    // -1 if the page is not allocated
    std::tuple<PageIndex<EnabledComponents>...> component_page_indices;
};
}
