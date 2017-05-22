#pragma once

#include <boost/preprocessor/comparison/less_equal.hpp>
#include <boost/preprocessor/tuple/to_seq.hpp>
#include <boost/preprocessor/seq/for_each.hpp>
#include <boost/vmd/is_empty.hpp>

#include "tuple.hpp"

#define YUKI_INFIX_JOIN_OP(r, pair_op_total, token) \
    token \
    BOOST_PP_IF(BOOST_PP_LESS_EQUAL(r, YUKI_PAIR_SECOND(pair_op_total)), YUKI_PAIR_FIRST(pair_op_total), BOOST_PP_EMPTY()) \
/**/

#define YUKI_INFIX_JOIN_IMPL_EMPTY(op, token_tuple) \
    BOOST_PP_EMPTY() \
/**/

#define YUKI_INFIX_JOIN_IMPL_NONEMPTY(op, token_tuple) \
    BOOST_PP_SEQ_FOR_EACH(YUKI_INFIX_JOIN_OP, YUKI_PAIR(op, BOOST_PP_TUPLE_SIZE(token_tuple)), BOOST_PP_TUPLE_TO_SEQ(token_tuple)) \
/**/

/**
* \brief Link tokens with another token as infix. e.g.
* YUKI_INFIX_JOIN(::, foo, bar, baz) becomes foo::bar::baz
* YUKI_INFIX_JOIN(::) becomes an empty token.
*/
#define YUKI_INFIX_JOIN(op, ...) \
    BOOST_PP_IF(BOOST_VMD_IS_EMPTY(__VA_ARGS__), YUKI_INFIX_JOIN_IMPL_EMPTY, YUKI_INFIX_JOIN_IMPL_NONEMPTY) \
        (op, (__VA_ARGS__)) \
/**/
