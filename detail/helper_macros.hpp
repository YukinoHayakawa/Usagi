#pragma once

#include <boost/preprocessor/facilities/overload.hpp>
#include <boost/preprocessor/control/if.hpp>

// expand member names to boost::mpl::vector arguments
#define YUKI_REFL_MEMBER_0()
#define YUKI_REFL_MEMBER_1(_1) YUKI_REFL_MEMBER(_1)
#define YUKI_REFL_MEMBER_2(_1, _2) YUKI_REFL_MEMBER_1(_1), YUKI_REFL_MEMBER(_2)
#define YUKI_REFL_MEMBER_3(_1, _2, _3) YUKI_REFL_MEMBER_2(_1, _2), YUKI_REFL_MEMBER(_3)
#define YUKI_REFL_MEMBER_4(_1, _2, _3, _4) YUKI_REFL_MEMBER_3(_1, _2, _3), YUKI_REFL_MEMBER(_4)
#define YUKI_REFL_MEMBER_5(_1, _2, _3, _4, _5) YUKI_REFL_MEMBER_4(_1, _2, _3, _4), YUKI_REFL_MEMBER(_5)
#define YUKI_REFL_MEMBER_6(_1, _2, _3, _4, _5, _6) YUKI_REFL_MEMBER_5(_1, _2, _3, _4, _5), YUKI_REFL_MEMBER(_6)
#define YUKI_REFL_MEMBER_7(_1, _2, _3, _4, _5, _6, _7) YUKI_REFL_MEMBER_6(_1, _2, _3, _4, _5, _6), YUKI_REFL_MEMBER(_7)
#define YUKI_REFL_MEMBER_8(_1, _2, _3, _4, _5, _6, _7, _8) YUKI_REFL_MEMBER_7(_1, _2, _3, _4, _5, _6, _7), YUKI_REFL_MEMBER(_8)
#define YUKI_REFL_MEMBER_9(_1, _2, _3, _4, _5, _6, _7, _8, _9) YUKI_REFL_MEMBER_8(_1, _2, _3, _4, _5, _6, _7, _8), YUKI_REFL_MEMBER(_9)
#define YUKI_REFL_MEMBER_10(_1, _2, _3, _4, _5, _6, _7, _8, _9, _10) YUKI_REFL_MEMBER_9(_1, _2, _3, _4, _5, _6, _7, _8, _9), YUKI_REFL_MEMBER(_10)
#define YUKI_REFL_MEMBER_11(_1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11) YUKI_REFL_MEMBER_10(_1, _2, _3, _4, _5, _6, _7, _8, _9, _10), YUKI_REFL_MEMBER(_11)
#define YUKI_REFL_MEMBER_12(_1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12) YUKI_REFL_MEMBER_11(_1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11), YUKI_REFL_MEMBER(_12)
#define YUKI_REFL_MEMBER_13(_1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13) YUKI_REFL_MEMBER_12(_1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12), YUKI_REFL_MEMBER(_13)
#define YUKI_REFL_MEMBER_14(_1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14) YUKI_REFL_MEMBER_13(_1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13), YUKI_REFL_MEMBER(_14)
#define YUKI_REFL_MEMBER_15(_1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15) YUKI_REFL_MEMBER_14(_1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14), YUKI_REFL_MEMBER(_15)
#define YUKI_REFL_MEMBER_16(_1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16) YUKI_REFL_MEMBER_15(_1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15), YUKI_REFL_MEMBER(_16)
#define YUKI_REFL_MEMBER_17(_1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17) YUKI_REFL_MEMBER_16(_1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16), YUKI_REFL_MEMBER(_17)
#define YUKI_REFL_MEMBER_18(_1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18) YUKI_REFL_MEMBER_17(_1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17), YUKI_REFL_MEMBER(_18)
#define YUKI_REFL_MEMBER_19(_1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19) YUKI_REFL_MEMBER_18(_1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18), YUKI_REFL_MEMBER(_19)
#define YUKI_REFL_MEMBER_20(_1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20) YUKI_REFL_MEMBER_19(_1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19), YUKI_REFL_MEMBER(_20)
#define YUKI_REFL_MEMBER_21(_1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, _21) YUKI_REFL_MEMBER_20(_1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20), YUKI_REFL_MEMBER(_21)
#define YUKI_REFL_MEMBER_22(_1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, _21, _22) YUKI_REFL_MEMBER_21(_1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, _21), YUKI_REFL_MEMBER(_22)
#define YUKI_REFL_MEMBER_23(_1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, _21, _22, _23) YUKI_REFL_MEMBER_22(_1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, _21, _22), YUKI_REFL_MEMBER(_23)
#define YUKI_REFL_MEMBER_24(_1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, _21, _22, _23, _24) YUKI_REFL_MEMBER_23(_1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, _21, _22, _23), YUKI_REFL_MEMBER(_24)
#define YUKI_REFL_MEMBER_25(_1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, _21, _22, _23, _24, _25) YUKI_REFL_MEMBER_24(_1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, _21, _22, _23, _24), YUKI_REFL_MEMBER(_25)
#define YUKI_REFL_MEMBER_26(_1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, _21, _22, _23, _24, _25, _26) YUKI_REFL_MEMBER_25(_1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, _21, _22, _23, _24, _25), YUKI_REFL_MEMBER(_26)
#define YUKI_REFL_MEMBER_27(_1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, _21, _22, _23, _24, _25, _26, _27) YUKI_REFL_MEMBER_26(_1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, _21, _22, _23, _24, _25, _26), YUKI_REFL_MEMBER(_27)
#define YUKI_REFL_MEMBER_28(_1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, _21, _22, _23, _24, _25, _26, _27, _28) YUKI_REFL_MEMBER_27(_1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, _21, _22, _23, _24, _25, _26, _27), YUKI_REFL_MEMBER(_28)
#define YUKI_REFL_MEMBER_29(_1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, _21, _22, _23, _24, _25, _26, _27, _28, _29) YUKI_REFL_MEMBER_28(_1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, _21, _22, _23, _24, _25, _26, _27, _28), YUKI_REFL_MEMBER(_29)
#define YUKI_REFL_MEMBER_30(_1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, _21, _22, _23, _24, _25, _26, _27, _28, _29, _30) YUKI_REFL_MEMBER_29(_1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, _21, _22, _23, _24, _25, _26, _27, _28, _29), YUKI_REFL_MEMBER(_30)
#define YUKI_REFL_MEMBER_31(_1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, _21, _22, _23, _24, _25, _26, _27, _28, _29, _30, _31) YUKI_REFL_MEMBER_30(_1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, _21, _22, _23, _24, _25, _26, _27, _28, _29, _30), YUKI_REFL_MEMBER(_31)
#define YUKI_REFL_MEMBER_32(_1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, _21, _22, _23, _24, _25, _26, _27, _28, _29, _30, _31, _32) YUKI_REFL_MEMBER_31(_1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, _21, _22, _23, _24, _25, _26, _27, _28, _29, _30, _31), YUKI_REFL_MEMBER(_32)

#if !BOOST_PP_VARIADICS_MSVC
#   define YUKI_REFL_MEMBER_LIST_EXPAND(...) \
        BOOST_PP_OVERLOAD(YUKI_REFL_MEMBER_, __VA_ARGS__)(__VA_ARGS__)
#else
#   define YUKI_REFL_MEMBER_LIST_EXPAND(...) \
        BOOST_PP_CAT(BOOST_PP_OVERLOAD(YUKI_REFL_MEMBER_, __VA_ARGS__)(__VA_ARGS__), BOOST_PP_EMPTY())
#endif

// overloaded member declaration list, the overload supports up to 32 members, but can be
// catenated with other sublists.
#define YUKI_REFL_MEMBERS(...) YUKI_REFL_MEMBER_LIST(YUKI_REFL_MEMBER_LIST_EXPAND(__VA_ARGS__))
#define YUKI_REFL_MEMBER_SUBLIST(...) YUKI_REFL_MEMBER_LIST_EXPAND(__VA_ARGS__)


#define YUKI_REFL_NESTED_TYPE_0()
#define YUKI_REFL_NESTED_TYPE_1(_1) YUKI_REFL_NESTED_TYPE(_1)
#define YUKI_REFL_NESTED_TYPE_2(_1, _2) YUKI_REFL_NESTED_TYPE_1(_1), YUKI_REFL_NESTED_TYPE(_2)
#define YUKI_REFL_NESTED_TYPE_3(_1, _2, _3) YUKI_REFL_NESTED_TYPE_2(_1, _2), YUKI_REFL_NESTED_TYPE(_3)
#define YUKI_REFL_NESTED_TYPE_4(_1, _2, _3, _4) YUKI_REFL_NESTED_TYPE_3(_1, _2, _3), YUKI_REFL_NESTED_TYPE(_4)
#define YUKI_REFL_NESTED_TYPE_5(_1, _2, _3, _4, _5) YUKI_REFL_NESTED_TYPE_4(_1, _2, _3, _4), YUKI_REFL_NESTED_TYPE(_5)
#define YUKI_REFL_NESTED_TYPE_6(_1, _2, _3, _4, _5, _6) YUKI_REFL_NESTED_TYPE_5(_1, _2, _3, _4, _5), YUKI_REFL_NESTED_TYPE(_6)
#define YUKI_REFL_NESTED_TYPE_7(_1, _2, _3, _4, _5, _6, _7) YUKI_REFL_NESTED_TYPE_6(_1, _2, _3, _4, _5, _6), YUKI_REFL_NESTED_TYPE(_7)
#define YUKI_REFL_NESTED_TYPE_8(_1, _2, _3, _4, _5, _6, _7, _8) YUKI_REFL_NESTED_TYPE_7(_1, _2, _3, _4, _5, _6, _7), YUKI_REFL_NESTED_TYPE(_8)
#define YUKI_REFL_NESTED_TYPE_9(_1, _2, _3, _4, _5, _6, _7, _8, _9) YUKI_REFL_NESTED_TYPE_8(_1, _2, _3, _4, _5, _6, _7, _8), YUKI_REFL_NESTED_TYPE(_9)
#define YUKI_REFL_NESTED_TYPE_10(_1, _2, _3, _4, _5, _6, _7, _8, _9, _10) YUKI_REFL_NESTED_TYPE_9(_1, _2, _3, _4, _5, _6, _7, _8, _9), YUKI_REFL_NESTED_TYPE(_10)
#define YUKI_REFL_NESTED_TYPE_11(_1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11) YUKI_REFL_NESTED_TYPE_10(_1, _2, _3, _4, _5, _6, _7, _8, _9, _10), YUKI_REFL_NESTED_TYPE(_11)
#define YUKI_REFL_NESTED_TYPE_12(_1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12) YUKI_REFL_NESTED_TYPE_11(_1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11), YUKI_REFL_NESTED_TYPE(_12)
#define YUKI_REFL_NESTED_TYPE_13(_1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13) YUKI_REFL_NESTED_TYPE_12(_1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12), YUKI_REFL_NESTED_TYPE(_13)
#define YUKI_REFL_NESTED_TYPE_14(_1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14) YUKI_REFL_NESTED_TYPE_13(_1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13), YUKI_REFL_NESTED_TYPE(_14)
#define YUKI_REFL_NESTED_TYPE_15(_1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15) YUKI_REFL_NESTED_TYPE_14(_1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14), YUKI_REFL_NESTED_TYPE(_15)
#define YUKI_REFL_NESTED_TYPE_16(_1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16) YUKI_REFL_NESTED_TYPE_15(_1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15), YUKI_REFL_NESTED_TYPE(_16)

#if !BOOST_PP_VARIADICS_MSVC
#   define YUKI_REFL_NESTED_TYPE_LIST_EXPAND(...) \
        BOOST_PP_OVERLOAD(YUKI_REFL_NESTED_TYPE_, __VA_ARGS__)(__VA_ARGS__)
#else
#   define YUKI_REFL_NESTED_TYPE_LIST_EXPAND(...) \
        BOOST_PP_CAT(BOOST_PP_OVERLOAD(YUKI_REFL_NESTED_TYPE_, __VA_ARGS__)(__VA_ARGS__), BOOST_PP_EMPTY())
#endif

// overloaded nested type declaration list
#define YUKI_REFL_NESTED_TYPES(...) YUKI_REFL_NESTED_TYPE_LIST(YUKI_REFL_NESTED_TYPE_LIST_EXPAND(__VA_ARGS__))
#define YUKI_REFL_NESTED_TYPE_SUBLIST(...) YUKI_REFL_NESTED_TYPE_LIST_EXPAND(__VA_ARGS__)
