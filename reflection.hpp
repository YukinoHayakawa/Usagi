#pragma once

/*
 * This file contains a set of macro for defining compile-time reflection information
 * for class and enum. The structure of definied information is conforming to std::class_traits
 * described in N4428 [1], with an extra field class_traits::identifier containing the
 * name of reflected class and nested in another namespace.
 * 
 * To use the macros, follow the example below:
 *
 * class A { };
 * class B { };
 * 
 * class C : public A, public virtual B
 * {
 * public:
 *     void foo() { }
 *     void bar() { }
 * 
 *     int baz;
 * 
 *     class D { };
 *     class E { };
 * 
 *     enum F { };
 * };
 * 
 * YUKI_REFL_CLASS_BEGIN(C)
 * YUKI_REFL_BASE_CLASSES(A, B)
 * YUKI_REFL_NESTED_TYPES(
 *     D, E, F
 * )
 * YUKI_REFL_MEMBERS(
 *     foo, bar,
 *     baz
 * )
 * YUKI_REFL_CLASS_END()
 * 
 * if the number of members exceeds the capacity of the overloaded macro,
 * use the manual macro with several sub-lists instead:
 * 
 * YUKI_REFL_MEMBER_LIST_M(
 *     YUKI_REFL_MEMBER_SUBLIST(foo, bar),
 *     YUKI_REFL_MEMBER_SUBLIST(baz)
 * )
 * YUKI_REFL_NESTED_TYPE_LIST_M(
 *     YUKI_REFL_NESTED_TYPE_SUBLIST(D, E, F)
 * )
 * 
 * [1] http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2015/n4428.pdf
 */

#include <boost/mpl/vector.hpp>
#include <boost/mpl/size.hpp>
#include <boost/mpl/at.hpp>
#include <boost/type_traits/is_virtual_base_of.hpp>
#include <boost/preprocessor/facilities/overload.hpp>

#include <Meta/char_list.hpp>

namespace yuki
{

template <typename T>
struct class_traits;

namespace detail
{

// todo: c++17 use auto for Ptr
template <typename P, P Ptr, typename NameCharList>
struct refl_class_member
{
    typedef NameCharList MemberName;
    static constexpr P member_pointer = Ptr;
};

template <typename T, typename IdentifierCharList>
struct refl_nested_type
{
    typedef IdentifierCharList NestedIdentifier;
    typedef T nested_type_t;
};

}

}

#define YUKI_REFL_CLASS_BEGIN(_class_t) \
namespace yuki { \
template <> struct class_traits<_class_t> { \
    private: typedef _class_t _reflecting_t; \
    public: static constexpr auto identifier = make_string_literal(#_class_t); \
/**/

#define YUKI_REFL_BASE_CLASSES(...) \
    private: typedef boost::mpl::vector<__VA_ARGS__> _base_list_t; \
/**/

#define YUKI_REFL_MEMBER(_member) \
    detail::refl_class_member<decltype(&_reflecting_t::_member), &_reflecting_t::_member, YUKI_MAKE_CHAR_LIST_STRINGIFY(_member)> \
/**/

#define YUKI_REFL_MEMBER_LIST(...) \
    private: typedef boost::mpl::vector<__VA_ARGS__> _member_list_t; \
/**/

#define YUKI_REFL_MEMBER(_member) \
    detail::refl_class_member<decltype(&_reflecting_t::_member), &_reflecting_t::_member, YUKI_MAKE_CHAR_LIST_STRINGIFY(_member)> \
/**/

#define YUKI_REFL_NESTED_TYPE_LIST(...) \
    private: typedef boost::mpl::vector<__VA_ARGS__> _nested_type_list_t; \
/**/

#define YUKI_REFL_NESTED_TYPE(_nested) \
    detail::refl_nested_type<_reflecting_t::_nested, YUKI_MAKE_CHAR_LIST_STRINGIFY(_nested)> \
/**/

#define YUKI_REFL_CLASS_END() \
    public: struct base_classes { \
        template <size_t I> struct get { \
            typedef typename boost::mpl::at<_base_list_t, boost::mpl::int_<I>>::type type; \
            static constexpr bool is_virtual = boost::is_virtual_base_of<type, _reflecting_t>::value; \
        }; \
        static constexpr size_t size = boost::mpl::size<_base_list_t>::type::value; \
    }; \
    public: struct class_members { \
        template <size_t I> struct get { \
            static constexpr auto name = boost::mpl::at<_member_list_t, boost::mpl::int_<I>>::type::MemberName::to_string_literal(); \
            static constexpr auto pointer = boost::mpl::at<_member_list_t, boost::mpl::int_<I>>::type::member_pointer; \
        }; \
        static constexpr size_t size = boost::mpl::size<_member_list_t>::type::value; \
    }; \
    public: struct nested_types { \
        template <size_t I> struct get { \
            static constexpr auto identifier = boost::mpl::at<_nested_type_list_t, boost::mpl::int_<I>>::type::NestedIdentifier::to_string_literal(); \
            typedef typename boost::mpl::at<_nested_type_list_t, boost::mpl::int_<I>>::type::nested_type_t type; \
        }; \
        static constexpr size_t size = boost::mpl::size<_nested_type_list_t>::type::value; \
    }; \
}; \
} \
/**/

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
#define YUKI_REFL_MEMBER_30(_1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, _21, _22, _nested_type_list_t_23, _24, _25, _26, _27, _28, _29, _30) YUKI_REFL_MEMBER_29(_1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, _21, _22, _23, _24, _25, _26, _27, _28, _29), YUKI_REFL_MEMBER(_30)
#define YUKI_REFL_MEMBER_31(_1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, _21, _22, _23, _24, _25, _26, _27, _28, _29, _30, _31) YUKI_REFL_MEMBER_30(_1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, _21, _22, _23, _24, _25, _26, _27, _28, _29, _30), YUKI_REFL_MEMBER(_31)
#define YUKI_REFL_MEMBER_32(_1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, _21, _22, _23, _24, _25, _26, _27, _28, _29, _30, _31, _32) YUKI_REFL_MEMBER_31(_1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, _21, _22, _23, _24, _25, _26, _27, _28, _29, _30, _31), YUKI_REFL_MEMBER(_32)

#if !BOOST_PP_VARIADICS_MSVC
#   define YUKI_REFL_MEMBER_LIST_EXPAND(...) \
        BOOST_PP_OVERLOAD(YUKI_REFL_MEMBER_, __VA_ARGS__)(__VA_ARGS__)
#else
#   define YUKI_REFL_MEMBER_LIST_EXPAND(...) \
        BOOST_PP_CAT(BOOST_PP_OVERLOAD(YUKI_REFL_MEMBER_, __VA_ARGS__)(__VA_ARGS__), BOOST_PP_EMPTY())
#endif

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

#define YUKI_REFL_NESTED_TYPES(...) YUKI_REFL_NESTED_TYPE_LIST(YUKI_REFL_NESTED_TYPE_LIST_EXPAND(__VA_ARGS__))
#define YUKI_REFL_NESTED_TYPE_SUBLIST(...) YUKI_REFL_NESTED_TYPE_LIST_EXPAND(__VA_ARGS__)
