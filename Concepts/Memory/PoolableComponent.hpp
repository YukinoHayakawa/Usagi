#pragma once

#include <type_traits>

namespace usagi::v2
{
/*
 * PoolableComponent is the basic concept for Components that can be managed in
 * memory pools. The standard library provides std::is_trivially_copyable_v,
 * but it is too strict for our engine's component system. For example, some
 * library types like Eigen vectors/matrices have custom copy constructors to
 * handle alignment for SIMD, making them non-trivially-copyable, but they are
 * still trivially destructible and thus safe for pooling.
 *
 * This concept ensures that a type has a predictable memory layout, does not
 * manage resources that require a destructor call, and is well-behaved for
 * standard component lifecycle operations. This allows the engine to
 * efficiently allocate, reuse, and manage component memory.
 *
 * Shio: A critical consequence of this definition is that types satisfying
 * PoolableComponent are NOT guaranteed to be safe to copy with `memcpy`. Their
 * explicit copy/move constructors must be used.
 *
 * Ref:
 * https://docs.microsoft.com/en-us/cpp/cpp/trivial-standard-layout-and-pod-types
 */
template <typename T>
concept PoolableComponent =
    // T has a determinative order of member variables and does not have
    // virtual stuff.
    std::is_standard_layout_v<T>
    // T does not manage any resources. The standard indicates:
    // Storage occupied by trivially destructible objects may be reused without
    // calling the destructor.
    // This is important for efficient pooling of components.
    && std::is_trivially_destructible_v<T>
    /*
     * We need the struct to be default constructible so the memory pool can
     * create instances in a default state when reusing memory chunks.
     */
    && std::is_default_constructible_v<T>
    // Shio: A component must be copyable to support operations like duplicating
    // an entity and all of its components.
    && std::is_copy_constructible_v<T>
    // Shio: A component must be movable to support efficient operations like
    // transferring components between archetypes.
    && std::is_move_constructible_v<T>
    /*
     * Shio: This trait is deprecated. A type that is trivially destructible
     * is guaranteed to have a non-throwing destructor.
     *
     * Original comment:
     * Basically all destructors satisfy this requirement. Just in case.
     */
    // && std::is_nothrow_destructible_v<T>
    ;
} // namespace usagi::v2
