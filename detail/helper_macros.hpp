#pragma once

#include <boost/preprocessor/seq/for_each.hpp>
#include <boost/preprocessor/tuple/to_seq.hpp>
#include <boost/preprocessor/tuple/size.hpp>

#define YUKI_REFL_DETAIL_MEMBERS(size, tuple) \
    YUKI_REFL_MEMBER_LIST(BOOST_PP_SEQ_FOR_EACH(YUKI_REFL_MEMBER, size, BOOST_PP_TUPLE_TO_SEQ(tuple))) \
/**/
#define YUKI_REFL_MEMBERS(...) \
    YUKI_REFL_DETAIL_MEMBERS(BOOST_PP_TUPLE_SIZE((__VA_ARGS__)), (__VA_ARGS__)) \
/**/

#define YUKI_REFL_DETAIL_NESTED_TYPES(size, tuple) \
    YUKI_REFL_NESTED_TYPE_LIST(BOOST_PP_SEQ_FOR_EACH(YUKI_REFL_NESTED_TYPE, size, BOOST_PP_TUPLE_TO_SEQ(tuple))) \
/**/
#define YUKI_REFL_NESTED_TYPES(...) \
    YUKI_REFL_DETAIL_NESTED_TYPES(BOOST_PP_TUPLE_SIZE((__VA_ARGS__)), (__VA_ARGS__)) \
/**/
