#pragma once

#include <tuple>
#include <utility>

#include <Usagi/Library/Meta/Functional/Callables.hpp>

namespace usagi::meta::reflection
{
/*
 * This function *unwraps* the nonstatic members of `T` with the provided
 * `OpUnWrap` and `OpProj` functions.
 */
// OpUnwrap and OpProj should both be callables. For them to work, you must use
// `reflect_constant` to get an actual function object's reflection.
template <
    typename T,
    // This MUST be a reflection of something callable.
    std::meta::info OpUnwrap,
    // So does this, and we ae using `std::identity()` as the default fallback.
    std::meta::info OpProj = std::meta::reflect_constant(std::identity())
>
constexpr auto unwrap_nonstatic_members(T & val)
{
    // I am yet to understand the differences between several types of
    // `access_context`. But it seems `current()` works fine here since we are
    // not manipulating any compound definitions.
    constexpr static auto ctx     = std::meta::access_context::current();
    // Use `define_static_array` to make `nonstatic_data_members_of` constexpr.
    // This is **required**, because otherwise `[:members[I]:]` cannot be
    // unpacked because its size is not constexpr.
    constexpr static auto members = std::define_static_array(
        std::meta::nonstatic_data_members_of(^^T, ctx)
    );
    // This is a general unwrapping function that supports value projection.
    constexpr static auto op_unwrap =
        [&]<std::size_t... I>(T & val, std::index_sequence<I...>) {
            return [:OpUnwrap:]([:OpProj:](val.[:members[I]:])...);
        };
    return op_unwrap(val, std::make_index_sequence<members.size()>());
}

template <typename T>
constexpr auto tie_nonstatic_members(T & val)
{
    return unwrap_nonstatic_members<
        T, std::meta::reflect_constant(functional::make_callable<^^std::tie>())
    >(val);
}

namespace static_tests
{
struct tie_member_test
{
    int  a;
    bool b;
};

constexpr tie_member_test tie_member_test_val = [] {
    tie_member_test temp;
    tie_nonstatic_members(temp) = std::make_tuple(31'415, false);
    return temp;
}();

static_assert(
    tie_member_test_val.a == 31'415 && tie_member_test_val.b == false
);
} // namespace static_tests
} // namespace usagi::meta::reflection
