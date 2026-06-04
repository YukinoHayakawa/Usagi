#pragma once

#include <type_traits>

namespace usagi
{
/*
 * Supposedly, a Component is a set of semantically coherent data put together
 * in one chunk so that CPU cache hit rate is happy. Mathematically, an Entity
 * is nothing but a set of unique Components with certain data values.
 *
 * In reality, there ain't no perfect cache hit. Components usually stem from
 * gameplay requirements and other game design bullshit. It's not playing a math
 * game. And you *will* hesitate about when and why to add a new Component. And,
 * sometimes things just fuck up, and you get bad performance.
 *
 * Usagi Engine's author believes that even though using memory mapped files as
 * persistence backend might be one of the worst ideas for any databases, but
 * still believes that it's better than implementing serialization. Because, if
 * C++ has zero-cost abstraction, why can't gamedevs have zero-cost save & load
 * of game states?
 *
 * Anyway, I want it. I'm tired of seeing major game engines' fucking load
 * screens like they don't know how to implement caches properly.
 *
 * Therefore, in Usagi Engine, a Component is strictly data-only, which means
 * no dynamic memory references, no non-trivial dtors, no other things that
 * prevent you from loading an EntityDatabase back straight from disk after
 * shutting down the engine and the game should stay in the same state except
 * for some practical concerns.
 *
 * This trait brings some extra benefits to the engine include:
 *
 * - It's relatively easy to port CPU code to, for example, GPU, or even
 *   executed remotely.
 * - No serialization needed at all unless you are going to deal with other
 *   protocols.
 * - Very low chance of memory bugs, as long as I don't fuck up implementing
 *   the memory allocators.
 * - Easy binding with GPU shader structs.
 *
 * Therefore, we gotta have some disciplines:
 *
 * - Standard layout. So you have predictable member offsets.
 * - Trivially destructible. Note that ctors are still allowed because, you will
 *   need them.
 * - Absolutely no references, pointers, or anything indirectly using them
 *   allowed.
 *
 * This might not be an exhaustive list of constraints.
 *
 * Note that in Usagi Engine v3 a struct is only regarded as the schema of a
 * Component, a Component is an instance of a schema.
 */
template <typename T>
concept ComponentSchema = requires {
    // Required for a predictable layout.
    requires std::is_standard_layout_v<T>;
    requires !std::is_pointer_v<T>;
    requires !std::is_reference_v<T>;
    // Required for moving data around.
    requires std::is_trivially_copyable_v<T>;
    // Things may behave differently on CPU/GPU. So nope.
    // On CPU side we may have more unnecessarily complicated operators.
    // requires std::is_trivially_copy_assignable_v<T>;
    // Similar idea.
    // requires std::is_trivially_copy_constructible_v<T>;
    // Required for memory block reuse.
    requires std::is_trivially_destructible_v<T>;
    // No fucking time to implement these right now.
    // TODO requires !meta::reflection::contains_pointers_or_references(^^T);
    // TODO requires !meta::reflection::contains_smart_pointers(^^T);
    // TODO requires !meta::reflection::contains_std_containers(^^T);
};
} // namespace usagi
