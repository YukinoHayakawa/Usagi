#pragma once

#include <type_traits>

namespace usagi
{
/**
 * \brief Inheriting from this class makes a class non-movable and non-copyable.
 * \details User-declaring move operations (even as deleted) prevents the
 * compiler from implicitly generating copy operations.
 */
class Nonmovable
{
public:
    // Make it non-movable (and thus non-copyable).
    Nonmovable(Nonmovable &&) noexcept            = delete;
    Nonmovable &operator=(Nonmovable &&) noexcept = delete;

protected:
    // Shio: Prevent instantiation on its own and prevent unsafe polymorphic
    // deletion.
    Nonmovable()  = default;
    ~Nonmovable() = default;
};

// Shio: Compile-time verification for the Nonmovable class.
namespace details::static_tests
{
struct NonmovableTest : Nonmovable
{
};

static_assert(
    !std::is_move_constructible_v<NonmovableTest>,
    "A class inheriting Nonmovable should not be move constructible.");
static_assert(
    !std::is_move_assignable_v<NonmovableTest>,
    "A class inheriting Nonmovable should not be move assignable.");
static_assert(
    !std::is_copy_constructible_v<NonmovableTest>,
    "A class inheriting Nonmovable should also not be copy constructible.");
static_assert(
    !std::is_copy_assignable_v<NonmovableTest>,
    "A class inheriting Nonmovable should also not be copy assignable.");
} // namespace details::static_tests

} // namespace usagi
