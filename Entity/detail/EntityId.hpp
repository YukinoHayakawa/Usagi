#pragma once

#include <cstdint>

namespace usagi
{
/**
 * \brief Entity Id is a 64-bit integer for uniquely identifying an Entity
 * in a specific Entity Database. Once an Entity is created (this is done
 * implicitly by inserting component at an empty slot in an Entity Page), its
 * Entity Id remains static until the page is destroyed. An Entity Page is
 * associated with an initial Entity Id which specifies the Id Range of the
 * page.
 *
 * Entity Id is not explicitly created for every Entity. The Systems managed
 * by the ECS architecture should never explicitly refer to any Entity, since
 * that would forbid the System being reused with another Database. The Systems
 * should only access Entities via Entity Iterator. (outdated)
 *
 * On the other hand, game logic outside the ECS architecture, such as input
 * handling and GUI, may want direct manipulation of some Entities. In such
 * cases, Entity Id can be used to obtain an Entity View from the Database.
 */
struct EntityId
{
    // The index of the Entity in the Entity Page
    std::uint64_t offset : 8 = -1;
    // The index of the Entity Page in the pool allocator.
    std::uint64_t page : 56 = -1;

    operator std::uint64_t() const
    {
        return *reinterpret_cast<const std::uint64_t*>(this);
    }
};
static_assert(sizeof(EntityId) == sizeof(std::uint64_t));
}
