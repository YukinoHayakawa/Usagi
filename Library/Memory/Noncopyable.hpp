#pragma once

#include <type_traits>

namespace usagi::v2
{
/**
 * \brief Inheriting from this class makes a class non-copyable.
 * \details A class that inherits from Noncopyable can still be movable.
 */
class Noncopyable
{
public:
    // Make it non-copyable
    Noncopyable(const Noncopyable &)            = delete;
    Noncopyable &operator=(const Noncopyable &) = delete;

    // Default move operations to allow derived classes to be movable.
    Noncopyable(Noncopyable &&) noexcept            = default;
    Noncopyable &operator=(Noncopyable &&) noexcept = default;

protected:
    // Shio: Prevent instantiation on its own and prevent unsafe polymorphic
    // deletion.
    Noncopyable()  = default;
    ~Noncopyable() = default;
};

// Shio: Compile-time verification for the Noncopyable class.
namespace details::static_tests
{
struct NoncopyableTest : Noncopyable
{
};

static_assert(
    !std::is_copy_constructible_v<NoncopyableTest>,
    "A class inheriting Noncopyable should not be copy constructible.");
static_assert(
    !std::is_copy_assignable_v<NoncopyableTest>,
    "A class inheriting Noncopyable should not be copy assignable.");
static_assert(
    std::is_move_constructible_v<NoncopyableTest>,
    "A class inheriting Noncopyable should remain move constructible.");
static_assert(
    std::is_move_assignable_v<NoncopyableTest>,
    "A class inheriting Noncopyable should remain move assignable.");
} // namespace details::static_tests

} // namespace usagi::v2
