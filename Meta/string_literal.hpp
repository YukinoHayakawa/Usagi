#pragma once

#include <cstddef>

namespace yuki
{

/**
 * \brief A temporary substitude for not-yet-standardized std::string_literal.
 * Wrap a compile time string literal excluding the terminating zero.
 */
template <typename T, size_t N>
struct basic_string_literal
{
    char data[N];

    /**
     * \brief Construct from a char array not terminated by 0.
     * \param arr 
     */
    constexpr basic_string_literal(const T (&arr)[N]) : data {}
    {
        for(size_t i = 0; i < N; ++i)
            data[i] = arr[i];
    }

    /**
    * \brief Construct from a C string.
    * \param arr
    */
    constexpr basic_string_literal(const T(&arr)[N + 1]) : data { }
    {
        for(size_t i = 0; i < N; ++i)
            data[i] = arr[i];
    }

    /**
     * \brief Compare with a zero-terminated string.
     * \tparam N2 
     * \param rhs 
     * \return 
     */
    template <size_t N2>
    constexpr bool operator ==(const T (&rhs)[N2]) const
    {
        if(N2 <= N) return false;
        for(size_t i = 0; i < N; ++i)
        {
            if(rhs[i] != data[i]) return false;
        }
        return rhs[N] == 0;
    }

    template <size_t N2>
    constexpr bool operator !=(const T(&rhs)[N2]) const
    {
        return !((*this) == rhs);
    }
};

template <char...Ch>
using string_literal = basic_string_literal<char, Ch...>;

/**
 * \brief Create string_literal from string literal.
 * \tparam N 
 * \param arr 
 * \return 
 */
template <size_t N>
constexpr auto make_string_literal(const char (&arr)[N])
{
    return string_literal<N - 1>(arr);
}

}
