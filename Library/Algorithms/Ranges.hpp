#pragma once

#include <array>
#include <ranges>

#include <Usagi/Library/Meta/Reflection/StaticReflection.hpp>

/*
 * `std::ranges` algorithms don't always work well in `consteval` contexts or
 * with `template for` etc. So sometimes we have to write some of our own
 * simplified versions doing the same things.
 */
namespace usagi::ranges
{
/*
 * Compares whether two ranges contain the same elements.
 */
template <typename RngL, typename RngR>
    requires (std::ranges::range<RngL> && std::ranges::range<RngR>)
// `Rng &&` with rvalue reference cannot satisfy `borrowed_range`
// constexpr bool is_equal(RngL && rng_l, RngR && rng_r)
constexpr bool is_equal(const RngL & rng_l, const RngR & rng_r)
{
    // quick exit for sized ranges
    if constexpr(
        std::ranges::sized_range<RngL> && std::ranges::sized_range<RngR>)
    {
        auto size_l = std::ranges::ssize(/*std::forward<RngL>*/(rng_l));
        auto size_r = std::ranges::ssize(/*std::forward<RngR>*/(rng_r));
        if(size_l != size_r) return false;
    }

    auto it_l  = std::ranges::begin(/*std::forward<RngL>*/(rng_l));
    auto it_r  = std::ranges::begin(/*std::forward<RngR>*/(rng_r));
    auto end_l = std::ranges::end(/*std::forward<RngL>*/(rng_l));
    auto end_r = std::ranges::end(/*std::forward<RngR>*/(rng_r));

    while(it_l != end_l && it_r != end_r)
    {
        // size different
        if(it_l == end_l && it_r != end_r) return false;
        if(it_l != end_l && it_r == end_r) return false;
        // value different
        if(*it_l != *it_r) return false;
        ++it_l;
        ++it_r;
    }

    return true;
}

/*
 * Sort elements putting unique elements in another container with the same type
 * of the input, returning the number of unique elements. This works in a
 * similar as `std::ranges::unique`. Duplicated elements are only considered in
 * a group of consecutive elements that contains elements of the same value.
 * Therefore, always pass an already-sorted range to this function.
 */
template <typename SrcRng, typename ContainerT = std::remove_cvref_t<SrcRng>>
    requires (std::ranges::random_access_range<SrcRng>)
constexpr std::pair<ContainerT, std::size_t> unique(SrcRng && src)
{
    ContainerT  dest { };
    std::size_t num_unique_elems = 0;
    std::size_t i                = 0;
    for(auto && e : src)
    {
        // push the first which is always unique.
        if(num_unique_elems == 0)
        {
            dest[num_unique_elems++] = src[i];
        }
        else
        {
            if(dest[num_unique_elems - 1] != src[i + 1])
            {
                dest[num_unique_elems++] = src[++i];
            }
            else
            {
                ++i;
            }
        }
    }
    return { dest, num_unique_elems };
}

namespace static_tests
{
consteval
{
    // Here, write some test cases
    constexpr static std::initializer_list      rng_1 { 1, 2, 3 };
    constexpr static std::initializer_list      rng_2 { 1, 2 };
    constexpr static std::initializer_list      rng_3 { 1, 2 };
    constexpr static std::array                 rng_4 { 1, 2, 3 };
    constexpr static std::initializer_list<int> rng_5 { };
    constexpr static std::initializer_list<int> rng_6 { };
    constexpr static std::array<int, 0>         rng_7 { };
    static_assert(is_equal(rng_1, rng_4));  // different types
    static_assert(is_equal(rng_2, rng_3));  // same type
    static_assert(!is_equal(rng_1, rng_2)); // same type
    static_assert(is_equal(rng_5, rng_6));  // same type
    static_assert(is_equal(rng_6, rng_7));  // different types
    static_assert(!is_equal(rng_1, rng_7)); // different types
    static_assert(!is_equal(rng_4, rng_7)); // same type

    constexpr static std::array rng_8 { 1, 1, 2, 2 };
    constexpr static std::array rng_9 { 1, 2, 0, 0 };
    // todo: constexpr structured binding P2686R5 not yet implemented in clang
    // constexpr auto [unique_dest, unique_num] = unique(rng_8);
    constexpr auto              unique_result = unique(rng_8);
    static_assert(is_equal(unique_result.first, rng_9));
    static_assert(unique_result.second == 2);
}
} // namespace static_tests
} // namespace usagi::ranges
