#pragma once

#include <array>
#include <cstdint>
#include <limits>
#include <immintrin.h>

#include <Usagi/Entity/Component.hpp>
#include <Usagi/Library/Meta/Tuple.hpp>

namespace usagi
{
// Type-irrelevant data of Entity Page.
struct EntityPageMeta
{
    // Page size = 32
    using EntityArrayT = std::uint32_t;

    // Entity index range 0-31
    using EntityIndexT = std::uint8_t;

    constexpr static std::size_t INVALID_PAGE = -1;
    constexpr static EntityIndexT PAGE_SIZE = 32;

    static_assert(PAGE_SIZE == std::numeric_limits<EntityArrayT>::digits);

    struct PageIndex // Wrapper class
    {
        std::uint64_t index = INVALID_PAGE;
    };

    struct EnabledMask
    {
        // The n-th bit is set if the n-th entity in this page has this
        // component
        EntityArrayT entity_array = 0;
    };
};

// todo: bit flag components shouldn't have page index field in entity page
template <Component... EnabledComponents>
struct EntityPage : EntityPageMeta
{
    // =========================== Metadata ============================= //

    // singly linked list for page iteration. probably should be atomic.
    std::uint64_t next_page = INVALID_PAGE;
    std::uint64_t page_seq_id = -1;

    // The index of first empty entity available for allocation.
    // This number only grows since entity ids are never reused.
    // Range 0-31
    // std::uint8_t first_unused_index = 0;

    // A mask indicating the positions having any components. An entity index
    // without any component is regarded as deleted and can be reused, since
    // it won't be accessed by any filter.
    // Corresponding bit is set to 0 when the entity is created. The mask is
    // recalculated during reclaim_pages().
    // todo: whether newly created entities will be accessed by filtered entity iterator depends on the relative positions of the iterator and the page had entities inserted.
    EntityArrayT free_mask = -1;

    // Any addition/removal of components or entities happened after previous
    // reset of this flag
    std::uint8_t dirty = false;

    // ============================ Types =============================== //

    constexpr static std::size_t NUM_COMPONENTS = sizeof...(EnabledComponents);

    using FilterT = ComponentFilter<EnabledComponents...>;

    // ========================== Components ============================ //

    std::array<EnabledMask, NUM_COMPONENTS> component_masks;

    // index into the page pool of each component.
    // -1 if the page is not allocated
    std::array<PageIndex, NUM_COMPONENTS> component_pages;

    // =========================== Accessors ============================ //

    EntityIndexT allocate()
    {
        const EntityIndexT first_free = _tzcnt_u32(free_mask);
        // If the page has no free slots, this assignment will not has any
        // effect since blsr will return 0 for 0.
        free_mask = _blsr_u32(free_mask);
        return first_free;
    }

    /* todo: the returned value is only to be trusted after dirty bit is cleared
    constexpr bool is_entity_empty(const EntityIndexT index)
    {
        return free_mask & (1u << index) != 0;
    }
    */

    template <Component C>
    constexpr static std::uint64_t component_index()
        requires FilterT::template HasComponent<C>
    {
        return Index<C, std::tuple<EnabledComponents...>>::value;
    }

    template <Component C>
    std::size_t & component_page_index()
    {
        return component_pages[component_index<C>()].index;
    }

    template <Component C>
    auto & component_enabled_mask()
    {
        return component_masks[component_index<C>()].entity_array;
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
