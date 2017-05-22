#pragma once

#include <boost/preprocessor/repetition/repeat.hpp>
#include <boost/preprocessor/punctuation/comma_if.hpp>

#include "apply_first_n_value.hpp"
#include "string_literal.hpp"

namespace yuki
{

/**
 * \brief A compile-time list of chars. This may be used as a constexpr string literal that
 * can be passed around as a template parameter.
 * \tparam Ch A list of T.
 */
template <typename T, T...Ch>
struct basic_char_list
{
private:
    /**
     * \brief Determine the length regarding the char list as a C-style string.
     * \return The number of chars before the first '\0', or sizeof...(Ch) is no '\0' is found.
     */
    static constexpr size_t _strlen()
    {
        for(size_t i = 0; i < sizeof...(Ch); ++i)
        {
            if(data[i] == 0) return i;
        }
        return sizeof...(Ch);
    }

public:
    static constexpr T data[sizeof...(Ch)] = { Ch... };

    /**
     * \brief Amount of elements in the char list.
     */
    static constexpr size_t length = sizeof...(Ch);
    /**
     * \brief Amount of character before the first '\0' or the total amount of characters if no '\0'
     * occurs in the list.
     */
    static constexpr size_t c_string_length = _strlen();

    /**
     * \brief The template type of basic_char_list<T>.
     * \tparam Ch2 Any other characters with type T.
     */
    template <T...Ch2>
    using homogeneous_list_t = basic_char_list<T, Ch2...>;

    typedef typename apply_first_n_value<c_string_length, T, homogeneous_list_t, Ch...>::type c_string_t;

    /**
    * \brief Cast the C string part to a string_literal.
    * \return A string_literal containing the C string part.
    */
    static constexpr basic_string_literal<T, c_string_length> to_string_literal()
    {
        return { c_string_t::data };
    }
};

template <char...Ch>
using char_list = basic_char_list<char, Ch...>;

namespace detail
{

template <typename Char, size_t N>
constexpr Char char_or_zero(Char(&Arr)[N], size_t i)
{
    return (i >= 0 && i < N ? Arr[i] : 0);
}

}
}

// a comma is prepended if N is not 0.
#define YUKI_STRCHR(z, N, string) BOOST_PP_COMMA_IF(N) ::yuki::detail::char_or_zero(string, N)

#define YUKI_MAKE_CHAR_LIST(string) \
    ::yuki::char_list< \
        BOOST_PP_REPEAT(16, YUKI_STRCHR, string) \
    >::c_string_t \
/**/

#define YUKI_MAKE_CHAR_LIST_STRINGIZE(token) \
    YUKI_MAKE_CHAR_LIST(#token) \
/**/
