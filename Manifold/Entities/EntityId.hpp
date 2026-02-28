#pragma once

#include <bit>
#include <cstdint>

#include <Usagi/Library/Memory/Casting.hpp>

namespace usagi
{
/**
 * @brief An EntityId represents a unique Entity existing in the EntityDatabase
 * and will never be reused even when the old Entity was destroyed and a new one
 * is allocated at the same memory location. The generation field is to prevent
 * referring to destroyed Entities.
 */
struct EntityId
{
    // The ID of the Component Group this Entity belongs to.
    // Component Groups allow the aggregated Entity Database to be sparse,
    // and the aggregation is transparently handled by the Executive.
    // 2^32 allows 4294967296 Component Groups, which is more than enough for
    // any kinds of games.
    std::uint32_t group_id      : 32 = -1;
    /*
     * Page index in the Entity metadata page allocator. This is bound to
     * Component Group.
     * 2^26 gives 67108864 Entity Pages per Component Group, combining with
     * 32-64 Entities per page, it's 2147483648-4294967296 Entities per
     * Component Group. Even one Component is only 64 bytes, this gives 256GB
     * of capabilities. I think this is enough for most of the games unless you
     * are to simulate the universe.
     */
    std::uint32_t metadata_page : 26 = -1;
    /*
     * Supports up to 64 Entities in a page. But the page allocator can
     * allocate fewer Entities, such as 32, in a page. For massive amount of
     * heterogeneous Entities, such as rays, intersections tests, material
     * evaluations, allocating larger pages may improve the performance.
     * But for quickly changing Entities such as mobs and players, making the
     * page too large would be a waste of memory.
     */
    std::uint32_t in_page_index : 6  = -1;
    /*
     * Disambiguation number for preventing referring to destroyed Entities.
     * If you run the game at 16ms per frame, 18446744073709600000*16ms equals
     * 9359078677.681158 years! Maybe I should save these bits for better use.
     */
    std::uint64_t generation         = -1;

    constexpr bool operator==(const EntityId & other) const
    {
        return reinterpret_primitive_type<__int128_t>(*this) ==
            reinterpret_primitive_type<__int128_t>(other);
    }

    constexpr bool operator!=(const EntityId & other) const = default;
};

static_assert(sizeof(EntityId) == sizeof(std::uint64_t) * 2,
    "EntityId must be exactly 128 bits");
} // namespace usagi
