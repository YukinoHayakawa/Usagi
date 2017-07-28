#pragma once

#include <boost/vmd/is_empty.hpp>
#include <boost/preprocessor/tuple/size.hpp>

#include "unpack.hpp"

#define YUKI_TUPLE_IS_EMPTY(tuple) \
    BOOST_VMD_IS_EMPTY(YUKI_UNPACK tuple) \
/**/

/**
 * \brief Get the amount of tuple elements, where an empty tuple consisting
 * only a pair of parentheses is considered as a tuple of 0 elements, instead
 * of which having one empty token.
 * https://svn.boost.org/trac/boost/ticket/7560
 * \param tuple 
 */
#define YUKI_TUPLE_SIZE(tuple) \
    BOOST_PP_IF(YUKI_TUPLE_IS_EMPTY(tuple), 0, BOOST_PP_TUPLE_SIZE(tuple)) \
/**/

#define YUKI_PAIR_FIRST_IMPL(x, y) x
#define YUKI_PAIR_SECOND_IMPL(x, y) y

#define YUKI_PAIR_FIRST(pair) YUKI_PAIR_FIRST_IMPL pair
#define YUKI_PAIR_SECOND(pair) YUKI_PAIR_SECOND_IMPL pair

#define YUKI_PAIR(x, y) (x, y)
