#pragma once

#include <boost/preprocessor/comparison/less_equal.hpp>
#include <boost/preprocessor/tuple/to_seq.hpp>
#include <boost/preprocessor/seq/for_each.hpp>
#include <boost/vmd/is_empty.hpp>

#include "tuple.hpp"
#include "transform.hpp"

#define YUKI_INFIX_JOIN_OP(r, op_total_transform, token) \
    BOOST_PP_TUPLE_ELEM(2, op_total_transform)(token) \
    BOOST_PP_IF(BOOST_PP_LESS_EQUAL(r, BOOST_PP_TUPLE_ELEM(1, op_total_transform)), BOOST_PP_TUPLE_ELEM(0, op_total_transform), BOOST_PP_EMPTY)() \
/**/

#define YUKI_INFIX_JOIN_IMPL_EMPTY(op, token_tuple, transform) \
    BOOST_PP_EMPTY() \
/**/

#define YUKI_INFIX_JOIN_IMPL_NONEMPTY(op, token_tuple, transform) \
    BOOST_PP_SEQ_FOR_EACH(YUKI_INFIX_JOIN_OP, (op, BOOST_PP_TUPLE_SIZE(token_tuple), transform), BOOST_PP_TUPLE_TO_SEQ(token_tuple)) \
/**/

/**
 * \brief Transfrom each token using transform_macro, then link then with op_macro.
 * \param op_macro The name of macro generating the infix operator.
 * \param transform_macro A macro having the form of MACRO(token).
 */
#define YUKI_TRANSFORM_INFIX_JOIN(op_macro, transform_macro, ...) \
    BOOST_PP_IF(BOOST_VMD_IS_EMPTY(__VA_ARGS__), YUKI_INFIX_JOIN_IMPL_EMPTY, YUKI_INFIX_JOIN_IMPL_NONEMPTY) \
        (op_macro, (__VA_ARGS__), transform_macro) \
/**/

 /**
 * \brief Link tokens with another token as infix. e.g.
 * \param op_macro The name of macro generating the infix operator.
 * YUKI_INFIX_JOIN(YUKI_OP_SCOPE, foo, bar, baz) becomes foo::bar::baz
 * YUKI_INFIX_JOIN(YUKI_OP_SCOPE) becomes an empty token.
 */
#define YUKI_INFIX_JOIN(op_macro, ...) \
    YUKI_TRANSFORM_INFIX_JOIN(op_macro, YUKI_ID, __VA_ARGS__) \
 /**/
