#pragma once

#include <bitset>
#include <array>

#include <Usagi/Experimental/ECS/Component/Component.hpp>

namespace usagi
{
template <
    std::size_t EntityPageSize,
    Component... EnabledComponents
>
struct alignas(64) EntityPage
{
    constexpr static std::size_t MAX_COMPONENTS = sizeof...(EnabledComponents);
    constexpr static std::size_t INVALID_PAGE = -1;

    // Entity Component Masks

    using ComponentMask = std::bitset<MAX_COMPONENTS>;
    struct ComponentState
    {
        // Bit mask of created components of an entity
        ComponentMask owned;
    };
    std::array<ComponentState, EntityPageSize> entity_states;

    // Component Storage References

    template <Component C>
    struct PageIndex
    {
        std::size_t index = INVALID_PAGE;
    };

    // index into the page pool of each component.
    // -1 if the page is not allocated
    std::tuple<PageIndex<EnabledComponents>...> component_page_indices;

    template <Component C>
    std::size_t & componentPageIndex()
    {
        return std::get<PageIndex<C>>(component_page_indices);
    }

    // Page Allocation Management


};
}
