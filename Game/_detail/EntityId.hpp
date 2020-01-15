#pragma once

#include <cstdint>

namespace usagi
{
/**
 * \brief The Entity Id is a 64-bit integer for uniquely identifying an Entity
 * in a specific Entity Database. Once an Entity is created (this is done
 * implicitly by inserting component at an empty slot in an Entity Page), its
 * Entity Id remains static until the page is destroyed. An Entity Page is
 * associated with an initial Entity Id which specifies the Id Range of the
 * page.
 *
 * Entity Id is not explicitly created for every Entity. The Systems managed
 * by the ECS architecture should never explicitly refer to any Entity, since
 * that would forbid the System being reused with another Database. The Systems
 * should only access Entities via Entity Iterator.
 *
 * On the other hand, game logic outside the ECS architecture, such as input
 * handling and GUI, may want direct manipulation of some Entities. In such
 * cases, Entity Id can be used to obtain an Entity View from the Database.
 */
struct EntityId
{
    std::uint64_t page_id;
    std::uint64_t id;
};
// static_assert(sizeof(EntityId) == sizeof(std::uint64_t));
}
