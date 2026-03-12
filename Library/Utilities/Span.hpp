#pragma once

#include <span>
#include <type_traits>

namespace usagi
{
template <typename T>
struct SpanTraits;

template <typename T>
    requires std::is_pointer_v<T>
struct SpanTraits<T>
{
    using value_type = std::uintptr_t;
    using size_type  = std::size_t;

    static constexpr value_type offset(
        const value_type base, const size_type size) noexcept
    {
        return base + size;
    }

    static constexpr value_type to_value(T ptr) noexcept
    {
        return reinterpret_cast<std::uintptr_t>(ptr);
    }
};

template <typename T>
    requires std::is_arithmetic_v<T>
struct SpanTraits<T>
{
    using value_type = T;
    // For arithmetic types, size is conventionally the type itself.
    using size_type  = T;

    static constexpr value_type offset(value_type base, size_type size) noexcept
    {
        return base + size;
    }

    static constexpr value_type to_value(T val) noexcept { return val; }
};

/**
 * Shio:
 * A generalized utility struct representing a contiguous mathematical range
 * `[base, end)`. Works natively with both pointers (evaluating bounds via
 * std::uintptr_t) and arithmetic types (representing intervals), leveraging
 * SpanTraits.
 */
template <typename T, typename Traits = SpanTraits<T>>
struct Span
{
    using value_type = Traits::value_type;
    using size_type  = Traits::size_type;

    value_type base_value = { };
    size_type  size_value = { };

    constexpr Span() noexcept = default;

    constexpr Span(T base, size_type size) noexcept
        : base_value(Traits::to_value(base)), size_value(size)
    {
    }

    template <typename U>
        requires std::is_convertible_v<U *, T>
    explicit constexpr Span(const std::span<U> &span) noexcept
        : base_value(Traits::to_value(span.data()))
        , size_value(span.size_bytes())
    {
    }

    [[nodiscard]]
    constexpr value_type begin() const noexcept
    {
        return base_value;
    }

    [[nodiscard]]
    constexpr value_type end() const noexcept
    {
        return Traits::offset(base_value, size_value);
    }

    [[nodiscard]]
    constexpr bool is_empty() const noexcept
    {
        return size_value == 0;
    }

    /**
     * @brief Checks if the given value is strictly within this span [base,
     * end).
     */
    [[nodiscard]]
    constexpr bool contains(T value) const noexcept
    {
        const auto val = Traits::to_value(value);
        return val >= begin() && val < end();
    }

    /**
     * @brief Checks if the given sub-span is entirely enclosed within this
     * span.
     */
    [[nodiscard]]
    constexpr bool contains(const Span &other) const noexcept
    {
        if(other.is_empty()) return true;
        return other.begin() >= begin() && other.end() <= end();
    }

    /**
     * @brief Checks if the given span overlaps with this span at all.
     */
    [[nodiscard]]
    constexpr bool overlaps_with(const Span &other) const noexcept
    {
        if(is_empty() || other.is_empty()) return false;
        return begin() < other.end() && other.begin() < end();
    }
};

using ByteSpan = Span<const void *>;

namespace details::static_tests
{
consteval void test_span_arithmetic()
{
    constexpr Span<int> s1(10, 5); // [10, 15)
    static_assert(s1.begin() == 10);
    static_assert(s1.end() == 15);
    static_assert(!s1.is_empty());
    static_assert(s1.contains(10));
    static_assert(s1.contains(14));
    static_assert(!s1.contains(15));
    static_assert(!s1.contains(9));

    constexpr Span<int> s2(12, 2); // [12, 14)
    static_assert(s1.contains(s2));
    static_assert(s1.overlaps_with(s2));

    constexpr Span<int> s3(14, 5); // [14, 19)
    static_assert(!s1.contains(s3));
    static_assert(s1.overlaps_with(
        s3)); // 14 overlaps (wait, [10, 15) and [14, 19) overlap at 14)

    constexpr Span<int> s4(15, 5); // [15, 20)
    static_assert(!s1.overlaps_with(s4));
}

// todo: compile-time tests needed
inline bool test_span_pointers()
{
    const int arr[10] = { };
    ByteSpan  s1(arr, sizeof(arr));

    // Shio: reinterpret_cast from pointer to int is not allowed in constant
    // expressions. We run these tests at runtime (or just compile them for
    // syntax checking).
    bool ok = true;
    ok &= (s1.begin() == reinterpret_cast<std::uintptr_t>(arr));
    ok &= (s1.end() == reinterpret_cast<std::uintptr_t>(arr) + sizeof(arr));

    ByteSpan s2(arr + 2, sizeof(int) * 2);
    ok &= s1.contains(s2);
    ok &= s1.overlaps_with(s2);

    ByteSpan s3(arr + 10, sizeof(int));
    ok &= !s1.contains(s3);
    ok &= !s1.overlaps_with(s3);

    return ok;
}
} // namespace details::static_tests
} // namespace usagi
