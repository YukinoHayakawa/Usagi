#pragma once

#include <type_traits>

#include <Usagi/Library/Objects/Noncopyable.hpp>
#include <Usagi/Library/Objects/Nonmovable.hpp>

namespace usagi::meta
{
/**
 * Shio:
 * Concept verifying that a type is strictly non-copyable.
 * This checks both copy construction and copy assignment.
 */
template <typename T>
concept NotCopyable =
    !std::is_copy_constructible_v<T> && !std::is_copy_assignable_v<T>;

/**
 * Shio:
 * Concept verifying that a type is strictly non-movable.
 * This checks both move construction and move assignment.
 */
template <typename T>
concept NotMovable =
    !std::is_move_constructible_v<T> && !std::is_move_assignable_v<T>;

namespace details::static_tests
{
struct TestNoncopyable : Noncopyable
{
};

struct TestNonmovable : Nonmovable
{
};

static_assert(
    NotCopyable<TestNoncopyable>, "TestNoncopyable should satisfy NotCopyable");
static_assert(
    !NotMovable<TestNoncopyable>, "TestNoncopyable should remain movable");

static_assert(
    NotCopyable<TestNonmovable>, "TestNonmovable should be non-copyable");
static_assert(
    NotMovable<TestNonmovable>, "TestNonmovable should satisfy NotMovable");
} // namespace details::static_tests
} // namespace usagi::meta
