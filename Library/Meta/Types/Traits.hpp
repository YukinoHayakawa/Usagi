#pragma once

#include <type_traits>

namespace usagi::type_traits
{
template <typename T>
concept CopyConstructible = std::is_copy_constructible_v<T>;

template <typename T>
concept CopyAssignable = std::is_copy_assignable_v<T>;

template <typename T>
concept Copyable = CopyConstructible<T> && CopyAssignable<T>;

template <typename T>
concept NotCopyConstructible = !std::is_copy_constructible_v<T>;

template <typename T>
concept NotCopyAssignable = !std::is_copy_assignable_v<T>;

template <typename T>
concept NotCopyable = NotCopyConstructible<T> && NotCopyAssignable<T>;

template <typename T>
concept MoveConstructible = std::is_move_constructible_v<T>;

template <typename T>
concept MoveAssignable = std::is_move_assignable_v<T>;

template <typename T>
concept Movable = MoveConstructible<T> && MoveAssignable<T>;

template <typename T>
concept NotMoveConstructible = !std::is_move_constructible_v<T>;

template <typename T>
concept NotMoveAssignable = !std::is_move_assignable_v<T>;

template <typename T>
concept NotMovable = NotMoveConstructible<T> && NotMoveAssignable<T>;

template <typename T>
concept Reference = std::is_reference_v<T>;

template <typename T>
concept NonReference = !std::is_reference_v<T>;
} // namespace usagi::type_traits
