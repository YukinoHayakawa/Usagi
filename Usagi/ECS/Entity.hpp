#pragma once

#include <bitset>

#include "Common.hpp"

namespace usagi::ecs
{
struct Entity
{
    using ComponentMask = std::bitset<MAX_COMPONENTS>;

    EntityId id;
    ComponentMask components;
    ComponentMask modified;

    Entity(EntityId id)
        : id(id)
    {
    }

    bool matchMask(const ComponentMask mask) const
    {
        return (mask & components) == mask;
    }

    // used in sorted array key comparisons

    friend bool operator<(const EntityId lhs, const Entity &rhs)
    {
        return lhs < rhs.id;
    }

    friend bool operator<(const Entity &lhs, const EntityId rhs)
    {
        return lhs.id < rhs;
    }
};
}
