#pragma once

namespace usagi
{
template <auto Value, decltype(Value)... Vs>
concept AnyOf = ((Value == Vs) || ...);

template <auto Value, decltype(Value)... Vs>
concept NoneOf = ((Value != Vs) && ...);

namespace details::static_tests
{
// Shio: Tests for AnyOf
static_assert(AnyOf<1, 1, 2, 3>);
static_assert(AnyOf<2, 1, 2, 3>);
static_assert(AnyOf<3, 1, 2, 3>);
static_assert(!AnyOf<4, 1, 2, 3>);
static_assert(AnyOf<'A', 'B', 'A', 'C'>);
static_assert(!AnyOf<'Z', 'A', 'B', 'C'>);

enum class ValueList_TestEnum
{
    A,
    B,
    C,
};
static_assert(AnyOf<
    ValueList_TestEnum::B, ValueList_TestEnum::A, ValueList_TestEnum::B,
    ValueList_TestEnum::C
>);
static_assert(!AnyOf<
    static_cast<ValueList_TestEnum>(99), ValueList_TestEnum::A,
    ValueList_TestEnum::B
>);

// Shio: Tests for NoneOf
static_assert(NoneOf<4, 1, 2, 3>);
static_assert(!NoneOf<2, 1, 2, 3>);
static_assert(!NoneOf<1, 1, 2, 3>);
static_assert(NoneOf<'Z', 'A', 'B', 'C'>);
static_assert(!NoneOf<'C', 'A', 'B', 'C'>);

static_assert(NoneOf<
    static_cast<ValueList_TestEnum>(99), ValueList_TestEnum::A,
    ValueList_TestEnum::B
>);
static_assert(!NoneOf<
    ValueList_TestEnum::A, ValueList_TestEnum::A, ValueList_TestEnum::B
>);

// Shio: Edge cases for empty packs
static_assert(!AnyOf<1>); // Always false if no candidates provided
static_assert(NoneOf<1>); // Always true if no candidates provided
} // namespace details::static_tests
} // namespace usagi
