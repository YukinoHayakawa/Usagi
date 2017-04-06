#pragma once

#include <boost/preprocessor/facilities/overload.hpp>
#include <boost/preprocessor/facilities/empty.hpp>

#define YUKI_INFIX_JOIN_1(_infix)
#define YUKI_INFIX_JOIN_2(_infix, _1) _1
#define YUKI_INFIX_JOIN_3(_infix, _1, _2) _1  _infix _2
#define YUKI_INFIX_JOIN_4(_infix, _1, _2, _3) YUKI_INFIX_JOIN_3(_infix, _1, _2)  _infix _3
#define YUKI_INFIX_JOIN_5(_infix, _1, _2, _3, _4) YUKI_INFIX_JOIN_4(_infix, _1, _2, _3)  _infix _4
#define YUKI_INFIX_JOIN_6(_infix, _1, _2, _3, _4, _5) YUKI_INFIX_JOIN_5(_infix, _1, _2, _3, _4)  _infix _5
#define YUKI_INFIX_JOIN_7(_infix, _1, _2, _3, _4, _5, _6) YUKI_INFIX_JOIN_6(_infix, _1, _2, _3, _4, _5)  _infix _6
#define YUKI_INFIX_JOIN_8(_infix, _1, _2, _3, _4, _5, _6, _7) YUKI_INFIX_JOIN_7(_infix, _1, _2, _3, _4, _5, _6)  _infix _7
#define YUKI_INFIX_JOIN_9(_infix, _1, _2, _3, _4, _5, _6, _7, _8) YUKI_INFIX_JOIN_8(_infix, _1, _2, _3, _4, _5, _6, _7)  _infix _8
#define YUKI_INFIX_JOIN_10(_infix, _1, _2, _3, _4, _5, _6, _7, _8, _9) YUKI_INFIX_JOIN_9(_infix, _1, _2, _3, _4, _5, _6, _7, _8)  _infix _9
#define YUKI_INFIX_JOIN_11(_infix, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10) YUKI_INFIX_JOIN_10(_infix, _1, _2, _3, _4, _5, _6, _7, _8, _9)  _infix _10
#define YUKI_INFIX_JOIN_12(_infix, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11) YUKI_INFIX_JOIN_11(_infix, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10)  _infix _11
#define YUKI_INFIX_JOIN_13(_infix, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12) YUKI_INFIX_JOIN_12(_infix, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11)  _infix _12
#define YUKI_INFIX_JOIN_14(_infix, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13) YUKI_INFIX_JOIN_13(_infix, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12)  _infix _13
#define YUKI_INFIX_JOIN_15(_infix, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14) YUKI_INFIX_JOIN_14(_infix, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13)  _infix _14
#define YUKI_INFIX_JOIN_16(_infix, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15) YUKI_INFIX_JOIN_15(_infix, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14)  _infix _15

#if !BOOST_PP_VARIADICS_MSVC
#   define YUKI_INFIX_JOIN_VA(...) \
        BOOST_PP_OVERLOAD(YUKI_INFIX_JOIN_, __VA_ARGS__)(__VA_ARGS__)
#else
#   define YUKI_INFIX_JOIN_VA(...) \
        BOOST_PP_CAT(BOOST_PP_OVERLOAD(YUKI_INFIX_JOIN_, __VA_ARGS__)(__VA_ARGS__), BOOST_PP_EMPTY())
#endif

/**
* \brief Link tokens with another token as infix. e.g.
* YUKI_INFIX_JOIN(::, foo, bar, baz) becomes foo::bar::baz
*/
#define YUKI_INFIX_JOIN(...) YUKI_INFIX_JOIN_VA(__VA_ARGS__)
