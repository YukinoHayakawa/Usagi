#pragma once

#include <Usagi/Utility/SortedArray.hpp>

#include "Common.hpp"

namespace usagi::ecs
{
/**
 * \brief Component storage.
 * \tparam ComponentT
 */
template <typename ComponentT>
struct ComponentStorage
{
    const std::size_t mask_bit = 0;

    ComponentStorage(std::size_t mask_bit)
        : mask_bit(mask_bit)
    {
    }

    struct ComponentInfo
    {
        EntityId entity = INVALID_ENTITY_ID;
        ComponentT component_data;
        bool pending_remove = false;

        // used in sorted array key comparisons

        friend bool operator<(
            const ComponentInfo &lhs,
            const ComponentInfo &rhs)
        {
            return lhs.entity < rhs.entity;
        }

        friend bool operator<(
            const EntityId lhs,
            const ComponentInfo &rhs)
        {
            return lhs < rhs.entity;
        }

        friend bool operator<(
            const ComponentInfo &lhs,
            const EntityId rhs)
        {
            return lhs.entity < rhs;
        }
    };

    SortedArray<ComponentInfo> components;
    using ComponentIterator = decltype(components)::template iterator;

    void insert(EntityId id, ComponentT comp)
    {
        // todo: batch insertions and removals
        components.insert({ id, std::move(comp) });
    }

    void erase(EntityId id)
    {
        components.erase(id);
    }
};
}
