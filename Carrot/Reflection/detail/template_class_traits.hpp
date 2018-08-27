#pragma once

#include <Usagi/Meta/char_list.hpp>
#include <Usagi/Preprocessor/identifier.hpp>

#include "trait_elements.hpp"
#include "class_traits_common.hpp"

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

#define YUKI_REFL_T_IMPL_MAKE_TYPE(_namespaces, _identifier) \
    template <YUKI_REFL_T_PARAM_LIST()> using _reflecting_t = YUKI_CANONICAL_CLASS_ID(_namespaces, _identifier)<YUKI_REFL_T_PARAM_NAME_LIST()> \
/**/
#define YUKI_REFL_T_IMPL_MAKE_CLASS_DECL(_namespaces, _identifier) \
    template <YUKI_REFL_T_PARAM_LIST()> struct template_class_traits<YUKI_CANONICAL_CLASS_ID(_namespaces, _identifier)<YUKI_REFL_T_PARAM_NAME_LIST()>> \
/**/
#define YUKI_REFL_T_IMPL_MAKE_CLASS_TYPE(_namespaces, _identifier) \
    typedef YUKI_CANONICAL_CLASS_ID(_namespaces, _identifier)<YUKI_REFL_T_PARAM_NAME_LIST()> _reflecting_t \
/**/

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
    YUKI_REFL_BASE( \
        _namespaces, _identifier, _elements, \
        YUKI_REFL_T_IMPL_MAKE_TYPE, YUKI_REFL_T_IMPL_MAKE_CLASS_DECL, YUKI_REFL_T_IMPL_MAKE_CLASS_TYPE \
    ) \
/**/

// specialization of default traits

/**
 * \brief Define the base classes of the template. Each base is enclosed with a pair
 * of parentheses, and the name of template parameters may be used in the name of
 * base classes.
 */
#define YUKI_REFL_T_BASE_CLASSES(...) \
    template <YUKI_REFL_T_PARAM_LIST()> struct base_list<_reflecting_t<YUKI_REFL_T_PARAM_NAME_LIST()>> { \
        typedef boost::mpl::vector<YUKI_TRANSFORM_INFIX_JOIN(BOOST_PP_COMMA, YUKI_TUPLE_TO_PARAM_LIST, BOOST_PP_EMPTY(), __VA_ARGS__)> _base_list_t; \
    }; \
/**/

#define YUKI_REFL_T_MEMBER_LIST(...) \
    template <YUKI_REFL_T_PARAM_LIST()> struct member_list<_reflecting_t<YUKI_REFL_T_PARAM_NAME_LIST()>> { \
        typedef boost::mpl::vector<__VA_ARGS__> _member_list_t; \
    }; \
/**/
#define YUKI_REFL_T_MEMBER(r, data, _member) \
    ::yuki::reflection::detail::class_member< \
        decltype(&_reflecting_t<YUKI_REFL_T_PARAM_NAME_LIST()>::_member), \
        &_reflecting_t<YUKI_REFL_T_PARAM_NAME_LIST()>::_member, \
        YUKI_MAKE_CHAR_LIST_STRINGIZE(_member) \
    > \
/**/
#define YUKI_REFL_T_MEMBERS(...) \
    YUKI_REFL_T_MEMBER_LIST(YUKI_TRANSFORM_INFIX_JOIN(BOOST_PP_COMMA, YUKI_REFL_T_MEMBER, BOOST_PP_EMPTY(), __VA_ARGS__)) \
/**/

#define YUKI_REFL_T_NESTED_TYPE_LIST(...) \
    template <YUKI_REFL_T_PARAM_LIST()> struct nested_type_list<_reflecting_t<YUKI_REFL_T_PARAM_NAME_LIST()>> { \
        typedef boost::mpl::vector<__VA_ARGS__> _nested_type_list_t; \
    }; \
/**/
#define YUKI_REFL_T_NESTED_TYPE(r, data, _identifier) \
    ::yuki::reflection::detail::nested_type< \
        typename _reflecting_t<YUKI_REFL_T_PARAM_NAME_LIST()>::_identifier, \
        YUKI_MAKE_CHAR_LIST_STRINGIZE(_identifier) \
    > \
/**/
#define YUKI_REFL_T_NESTED_TYPES(...) \
    YUKI_REFL_T_NESTED_TYPE_LIST(YUKI_TRANSFORM_INFIX_JOIN(BOOST_PP_COMMA, YUKI_REFL_T_NESTED_TYPE, BOOST_PP_EMPTY(), __VA_ARGS__)) \
/**/
