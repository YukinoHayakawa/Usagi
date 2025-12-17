#pragma once

#include <algorithm>
#include <string>
#include <string_view>

namespace usagi
{
template <std::size_t N>
class fixed_string
{
    char        buf[N + 1] { }; // Buffer capacity + 1 for null terminator
    std::size_t m_length { 0 }; // Actual length of the string

public:
    // 1. Literal Constructor (Exact fit)
    // "Hello" (char[6]) -> fixed_string<5>
    constexpr fixed_string(const char (&s)[N + 1])
        : m_length(N)
    {
        std::copy_n(s, N, buf);
        buf[N] = '\0';
    }

    // 2. string_view Constructor (Variable fit up to N)
    constexpr explicit fixed_string(std::string_view s)
    {
        // "static_assert" replacement:
        // Shio: In constexpr/consteval, throwing an exception is a compile-time
        // error.
        // todo: consteval throwing P3068R6 not yet implemented in clang
        if(s.size() > N)
        {
            throw "Input string exceeds fixed_string capacity";
        }

        std::copy_n(s.data(), s.size(), buf);
        m_length = s.size();

        // Correctly place null terminator at the end of the actual data
        buf[m_length] = '\0';
    }

    // Default Constructor
    constexpr fixed_string() { buf[0] = '\0'; }

    // Accessors
    constexpr std::size_t size() const { return m_length; }

    constexpr std::size_t length() const { return m_length; }

    constexpr const char * c_str() const { return buf; }

    constexpr auto operator<=>(const fixed_string &) const = default;

    // Helper to get a view for easy comparison
    constexpr std::string_view view() const
    {
        return std::string_view(buf, m_length);
    }

    constexpr std::string to_string() const { return std::string { view() }; }

    // -------------------------------------------------------------------------
    // Comparison Operators (Handling different N)
    // -------------------------------------------------------------------------

    // Equality: Allows comparing fixed_string<N> with fixed_string<M>
    template <std::size_t M>
    constexpr bool operator==(const fixed_string<M> & rhs) const
    {
        return view() == rhs.view();
    }

    template <std::size_t N1, std::size_t N2>
    friend constexpr auto
        operator+(const fixed_string<N1> & lhs, const fixed_string<N2> & rhs);
};

// Deduction Guide: char[N] -> fixed_string<N-1>
template <std::size_t N>
fixed_string(const char (&)[N]) -> fixed_string<N - 1>;

template <std::size_t N1, std::size_t N2>
constexpr auto
    operator+(const fixed_string<N1> & lhs, const fixed_string<N2> & rhs)
{
    // Result capacity is sum of capacities (conservative approach)
    fixed_string<N1 + N2> result;

    // 1. Copy LHS
    std::copy_n(lhs.buf, lhs.size(), result.buf);

    // 2. Append RHS
    std::copy_n(rhs.buf, rhs.size(), result.buf + lhs.size());

    // 3. Set new length and terminate
    result.m_length             = lhs.size() + rhs.size();
    result.buf[result.m_length] = '\0';

    return result;
}

// Helper: Ensure we allocate exactly enough space for the view
template <std::size_t N>
constexpr auto make_fixed_string(std::string_view s)
{
    return fixed_string<N>(s);
}

namespace static_tests
{
consteval
{
    // -------------------------------------------------------------------------
    // Case 1: Partial Fill (Capacity > Size)
    // -------------------------------------------------------------------------
    constexpr std::string_view small_view = "Hi";
    // Capacity 10, Content "Hi" (len 2)
    constexpr fixed_string<10> fs_small(small_view);

    static_assert(fs_small.size() == 2, "Size should be 2, not 10");
    static_assert(fs_small.c_str()[0] == 'H');
    static_assert(
        fs_small.c_str()[2] == '\0', "Null terminator misplaced at index 2"
    );
    // Ensure the rest of the buffer didn't interfere (though undefined usually,
    // we init to 0)
    static_assert(fs_small.c_str()[9] == '\0');

    // -------------------------------------------------------------------------
    // Case 2: Exact Fill
    // -------------------------------------------------------------------------
    constexpr fixed_string fs_exact = "Hello"; // Deduces <5>
    static_assert(fs_exact.size() == 5);
    static_assert(fs_exact.c_str()[5] == '\0');

    // -------------------------------------------------------------------------
    // Case 3: Concatenation Logic
    // -------------------------------------------------------------------------
    constexpr fixed_string<5> part1("He");              // len 2
    constexpr fixed_string<5> part2("llo");             // len 3
    constexpr auto            combined = part1 + part2; // Capacity 10, Size 5

    static_assert(combined.size() == 5);
    static_assert(combined.c_str()[0] == 'H');
    static_assert(combined.c_str()[5] == '\0'); // Terminator at index 5
    // Sanity check: ensure it matches literal
    static_assert(combined == fixed_string("Hello"));
    static_assert(!(combined != fixed_string("Hello")));

    // -------------------------------------------------------------------------
    // Case 4: Compile-Time Prohibition (Uncommenting lines below causes error)
    // -------------------------------------------------------------------------

    /*
    constexpr std::string_view too_big = "ThisStringIsTooLong";
    // Error: "Input string exceeds fixed_string capacity"
    constexpr fixed_string<5>  fail(too_big);
    */
}
} // namespace static_tests
} // namespace usagi
