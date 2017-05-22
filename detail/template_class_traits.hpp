#pragma once

#include <boost/mpl/vector.hpp>
#include <boost/mpl/size.hpp>
#include <boost/mpl/at.hpp>
#include <boost/type_traits/is_virtual_base_of.hpp>
#include <boost/preprocessor/seq/to_tuple.hpp>

#include <Usagi/Meta/char_list.hpp>
#include <Usagi/Preprocessor/infix_join.hpp>
#include <Usagi/Preprocessor/make_nested_namespace.hpp>
#include <Usagi/Preprocessor/identifier.hpp>
#include <Usagi/Preprocessor/op.hpp>
#include <Usagi/Preprocessor/functional.hpp>

namespace yuki
{
namespace reflection
{
namespace detail
{

template <typename>
struct template_class_traits { };

}
}
}

#define YUKI_REFL_T_PARAM_LIST() YUKI_UNPACK YUKI_PAIR_FIRST(YUKI_REFL_T_PARAMS())
#define YUKI_REFL_T_PARAM_NAME_LIST() YUKI_UNPACK YUKI_PAIR_SECOND(YUKI_REFL_T_PARAMS())

/**
 * \brief Define the reflection data for a template.
 * 
 * e.g.
 * 
 * template <typename T, typename U>
 * class TA { };
 * class TB { };
 * template <typename T>
 * class T1 { };
 * 
 * template <typename T, typename U, template <typename> typename TT, int I>
 * struct TC : TA<T, U>, virtual TB
 * {
 *     void foo() { }
 *     void bar() { }
 * 
 *     T baz;
 * 
 *     class D { };
 *     class E { };
 * 
 *     enum F { };
 * };
 * 
 * #define YUKI_REFL_T_PARAMS() \
 * ((typename T, typename U, template <typename> typename TT, int I), (T, U, TT, I))
 * YUKI_REFL_TEMPLATE((), TC, (
 *     YUKI_REFL_T_BASE_CLASSES((TA<T,U>), (TB))
 *     YUKI_REFL_T_NESTED_TYPES(D, E, F)
 *     YUKI_REFL_T_MEMBERS(foo, bar, baz)
 * ))
 * #undef YUKI_REFL_T_PARAMS
 * 
 * YUKI_REFL_T_PARAMS must be defined as a pair of two tuples, where the first contains
 * the list of template parameter list, the second contains the names of them.
 * 
 * \param _namespaces A tuple of the full namespace the template is in. e.g. (foo, bar)
 * \param _identifier The name of template.
 * \param _elements Define the information of the template using macros enclosed in a
 * pair of parentheses. Available macros include:
 * YUKI_REFL_T_BASE_CLASSES, YUKI_REFL_T_MEMBERS, YUKI_REFL_T_NESTED_TYPES.
 */
#define YUKI_REFL_TEMPLATE(_namespaces, _identifier, _elements) \
namespace yuki { namespace reflection { namespace detail { namespace meta { YUKI_MAKE_NESTED_NAMESPACE((YUKI_HEAD_UNPACK _namespaces _identifier), (\
YUKI_USE_NAMESPACE(_namespaces) /* introduce identifiers from the namespace containing the class */ \
template <YUKI_REFL_T_PARAM_LIST()> using _reflecting_t = YUKI_CANONICAL_CLASS_ID(_namespaces, _identifier)<YUKI_REFL_T_PARAM_NAME_LIST()>; \
/* default traits, if user does not use the corresponding defining macros, these will be used */ \
template <typename T> struct base_list { typedef boost::mpl::vector<> _base_list_t; }; \
template <typename T> struct member_list { typedef boost::mpl::vector<> _member_list_t; }; \
template <typename T> struct nested_type_list { typedef boost::mpl::vector<> _nested_type_list_t; }; \
YUKI_UNPACK _elements /* template specializations, inside which only _reflecting_t is used */ \
)) }}}} /* namespace yuki::reflection::detail::meta */ \
namespace yuki { namespace reflection { namespace detail { \
template <YUKI_REFL_T_PARAM_LIST()> struct template_class_traits<YUKI_CANONICAL_CLASS_ID(_namespaces, _identifier)<YUKI_REFL_T_PARAM_NAME_LIST()>> { \
private: \
    typedef YUKI_CANONICAL_CLASS_ID(_namespaces, _identifier)<YUKI_REFL_T_PARAM_NAME_LIST()> _reflecting_t; \
    /* use YUKI_HEAD_UNPACK instaed of YUKI_TAIL_UNPACK, or MSVC will consider YUKI_OP_SCOPE YUKI_TAIL_UNPACK _namespaces as a single argument */ \
    typedef typename meta::YUKI_INFIX_JOIN(YUKI_OP_SCOPE, YUKI_HEAD_UNPACK _namespaces _identifier, base_list<_reflecting_t>::_base_list_t) _base_list_t; \
    typedef typename meta::YUKI_INFIX_JOIN(YUKI_OP_SCOPE, YUKI_HEAD_UNPACK _namespaces _identifier, member_list<_reflecting_t>::_member_list_t) _member_list_t; \
    typedef typename meta::YUKI_INFIX_JOIN(YUKI_OP_SCOPE, YUKI_HEAD_UNPACK _namespaces _identifier, nested_type_list<_reflecting_t>::_nested_type_list_t) _nested_type_list_t; \
public: \
    static constexpr auto identifier = make_string_literal(#_identifier); \
    struct base_classes : base_classes_base<_reflecting_t, _base_list_t> { }; \
    struct class_members : class_members_base<_member_list_t> { }; \
    struct nested_types : nested_types_base<_nested_type_list_t> { }; \
}; \
}}} /* namespace yuki::reflection::detail */ \
/**/

// specialization of default traits

/**
 * \brief Define the base classes of the template. Each base is enclosed with a pair
 * of parentheses, and the name of template parameters may be used in the name of
 * base classes.
 */
#define YUKI_REFL_T_BASE_CLASSES(...) \
    template <YUKI_REFL_T_PARAM_LIST()> struct base_list<_reflecting_t<YUKI_REFL_T_PARAM_NAME_LIST()>> { \
        typedef boost::mpl::vector<YUKI_TRANSFORM_INFIX_JOIN(BOOST_PP_COMMA, YUKI_TUPLE_TO_PARAM_LIST, __VA_ARGS__)> _base_list_t; \
    }; \
/**/

#define YUKI_REFL_T_MEMBER_LIST(...) \
    template <YUKI_REFL_T_PARAM_LIST()> struct member_list<_reflecting_t<YUKI_REFL_T_PARAM_NAME_LIST()>> { \
        typedef boost::mpl::vector<__VA_ARGS__> _member_list_t; \
    }; \
/**/
#define YUKI_REFL_T_MEMBER(_member) \
    ::yuki::reflection::detail::class_member< \
        decltype(&_reflecting_t<YUKI_REFL_T_PARAM_NAME_LIST()>::_member), \
        &_reflecting_t<YUKI_REFL_T_PARAM_NAME_LIST()>::_member, \
        YUKI_MAKE_CHAR_LIST_STRINGIZE(_member) \
    > \
/**/
#define YUKI_REFL_T_MEMBERS(...) \
    YUKI_REFL_T_MEMBER_LIST(YUKI_TRANSFORM_INFIX_JOIN(BOOST_PP_COMMA, YUKI_REFL_T_MEMBER, __VA_ARGS__)) \
/**/

#define YUKI_REFL_T_NESTED_TYPE_LIST(...) \
    template <YUKI_REFL_T_PARAM_LIST()> struct nested_type_list<_reflecting_t<YUKI_REFL_T_PARAM_NAME_LIST()>> { \
        typedef boost::mpl::vector<__VA_ARGS__> _nested_type_list_t; \
    }; \
/**/
#define YUKI_REFL_T_NESTED_TYPE(_identifier) \
    ::yuki::reflection::detail::nested_type< \
        typename _reflecting_t<YUKI_REFL_T_PARAM_NAME_LIST()>::_identifier, \
        YUKI_MAKE_CHAR_LIST_STRINGIZE(_identifier) \
    > \
/**/
#define YUKI_REFL_T_NESTED_TYPES(...) \
    YUKI_REFL_T_NESTED_TYPE_LIST(YUKI_TRANSFORM_INFIX_JOIN(BOOST_PP_COMMA, YUKI_REFL_T_NESTED_TYPE, __VA_ARGS__)) \
/**/
