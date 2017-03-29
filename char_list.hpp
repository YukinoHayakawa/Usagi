#pragma once

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

}

#define YUKI_STRCHR(string, N) ((N >= 0 && N < sizeof(string)) ? string[N] : 0)

#define YUKI_MAKE_CHAR_LIST(string) \
    ::yuki::char_list< \
        YUKI_STRCHR(string, 0),  YUKI_STRCHR(string, 1),  YUKI_STRCHR(string, 2),  YUKI_STRCHR(string, 3),  \
        YUKI_STRCHR(string, 4),  YUKI_STRCHR(string, 5),  YUKI_STRCHR(string, 6),  YUKI_STRCHR(string, 7),  \
        YUKI_STRCHR(string, 8),  YUKI_STRCHR(string, 9),  YUKI_STRCHR(string, 10), YUKI_STRCHR(string, 11), \
        YUKI_STRCHR(string, 12), YUKI_STRCHR(string, 13), YUKI_STRCHR(string, 14), YUKI_STRCHR(string, 15), \
        YUKI_STRCHR(string, 16), YUKI_STRCHR(string, 17), YUKI_STRCHR(string, 18), YUKI_STRCHR(string, 19), \
        YUKI_STRCHR(string, 20), YUKI_STRCHR(string, 21), YUKI_STRCHR(string, 22), YUKI_STRCHR(string, 23), \
        YUKI_STRCHR(string, 24), YUKI_STRCHR(string, 25), YUKI_STRCHR(string, 26), YUKI_STRCHR(string, 27), \
        YUKI_STRCHR(string, 28), YUKI_STRCHR(string, 29), YUKI_STRCHR(string, 30), YUKI_STRCHR(string, 31), \
        YUKI_STRCHR(string, 32), YUKI_STRCHR(string, 33), YUKI_STRCHR(string, 34), YUKI_STRCHR(string, 35), \
        YUKI_STRCHR(string, 36), YUKI_STRCHR(string, 37), YUKI_STRCHR(string, 38), YUKI_STRCHR(string, 39), \
        YUKI_STRCHR(string, 40), YUKI_STRCHR(string, 41), YUKI_STRCHR(string, 42), YUKI_STRCHR(string, 43), \
        YUKI_STRCHR(string, 44), YUKI_STRCHR(string, 45), YUKI_STRCHR(string, 46), YUKI_STRCHR(string, 47), \
        YUKI_STRCHR(string, 48), YUKI_STRCHR(string, 49), YUKI_STRCHR(string, 50), YUKI_STRCHR(string, 51), \
        YUKI_STRCHR(string, 52), YUKI_STRCHR(string, 53), YUKI_STRCHR(string, 54), YUKI_STRCHR(string, 55), \
        YUKI_STRCHR(string, 56), YUKI_STRCHR(string, 57), YUKI_STRCHR(string, 58), YUKI_STRCHR(string, 59), \
        YUKI_STRCHR(string, 60), YUKI_STRCHR(string, 61), YUKI_STRCHR(string, 62), YUKI_STRCHR(string, 63)  \
    >::c_string_t \
/**/

#define YUKI_MAKE_CHAR_LIST_STRINGIFY(token) \
    YUKI_MAKE_CHAR_LIST(#token) \
/**/
