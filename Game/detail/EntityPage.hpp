#pragma once

#include <tuple>

#include <Usagi/Game/Entity/Component.hpp>

namespace usagi
{
template <Component... EnabledComponents>
struct EntityPage
{
    // ============================ Types ============================== //

    // Page size = 32
    using EntityArrayT = std::uint32_t;

    // Entity index range 0-31
    using EntityIndexT = std::uint8_t;

    constexpr static std::size_t INVALID_PAGE = -1;
    constexpr static EntityIndexT PAGE_SIZE = 32;

    static_assert(PAGE_SIZE == CHAR_BIT * sizeof EntityArrayT);

    template <Component C>
    struct PageIndex // Wrapper class
    {
        std::uint64_t index = INVALID_PAGE;
    };

    template <Component C>
    struct ComponentEnabledMask
    {
        // The n-th bit is set if the n-th entity in this page has this
        // component
        EntityArrayT entity_array = 0;
    };

    // =========================== Metadata ============================= //

    // singly linked list for page iteration. probably should be atomic.
    std::uint64_t next_page = INVALID_PAGE;

    // The id of the first entity in this page
    std::uint64_t first_entity_id = -1;

    // The index of first empty entity available for allocation.
    // This number only grows since entity ids are never reused.
    // Range 0-31
    std::uint8_t first_unused_index = 0;

    // Any addition/removal of components or entities happened after previous
    // reset of this flag
    std::uint8_t dirty = false;

    // ========================== Components ============================ //

    std::tuple<ComponentEnabledMask<EnabledComponents>...> component_masks;

    // index into the page pool of each component.
    // -1 if the page is not allocated
    std::tuple<PageIndex<EnabledComponents>...> component_pages;

    // =========================== Helpers ============================= //

    template <Component C>
    std::size_t & component_page_index()
    {
        return std::get<PageIndex<C>>(component_pages).index;
    }

    template <Component C>
    auto & component_enabled_mask()
    {
        return std::get<ComponentEnabledMask<C>>(component_masks).entity_array;
    }

    template <Component C>
    void set_component_bit(const EntityIndexT index)
    {
        // ideally a bts instruction
        component_enabled_mask<C>() |= 1u << index;
    }

    template <Component C>
    void reset_component_bit(const EntityIndexT index)
    {
        // ideally a btr instruction
        component_enabled_mask<C>() &= ~(1u << index);
    }

    template <Component C>
    bool component_bit(const EntityIndexT index)
    {
        return component_enabled_mask<C>() & (1u << index);
    }
};
}
