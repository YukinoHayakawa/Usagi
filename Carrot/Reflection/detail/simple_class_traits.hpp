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
struct simple_class_traits { };

}
}
}

#define YUKI_REFL_IMPL_MAKE_TYPE(_namespaces, _identifier) \
    typedef YUKI_CANONICAL_CLASS_ID(_namespaces, _identifier) _reflecting_t \
/**/
#define YUKI_REFL_IMPL_MAKE_CLASS_DECL(_namespaces, _identifier) \
    template <> struct simple_class_traits<YUKI_CANONICAL_CLASS_ID(_namespaces, _identifier)> \
/**/

/**
 * \brief Main macro for defining class reflection information. Invoke this macro in the
 * root namespace then access reflection info through yuki::class_traits<T>.
 * 
 * e.g.
 * 
 * namespace foo { namespace bar {
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
 * }}
 *
 * YUKI_REFL_CLASS((foo, bar), C, (
 *     YUKI_REFL_BASE_CLASSES(A, B)
 *     YUKI_REFL_NESTED_TYPES(D, E, F)
 *     YUKI_REFL_MEMBERS(foo, bar, baz)
 * ))
 * 
 * \param _namespace The namespace in which the class is declared, separated into unqualified
 * identifiers and surrounded by a pair of parentheses.
 * \param _identifier The unqualified name of reflected class.
 * \param _elements Optional elements enclosed in a pair of (). Available elements include
 * YUKI_REFL_BASE_CLASSES, YUKI_REFL_NESTED_TYPES, and YUKI_REFL_MEMBERS.
 */
#define YUKI_REFL_CLASS(_namespaces, _identifier, _elements) \
    YUKI_REFL_BASE( \
        _namespaces, _identifier, _elements, \
        YUKI_REFL_IMPL_MAKE_TYPE, YUKI_REFL_IMPL_MAKE_CLASS_DECL, YUKI_REFL_IMPL_MAKE_TYPE \
    ) \
/**/

// specialization of default traits

#define YUKI_REFL_BASE_CLASSES(...) \
    template <> struct base_list<_reflecting_t> { typedef boost::mpl::vector<__VA_ARGS__> _base_list_t; }; \
/**/

#define YUKI_REFL_MEMBER_LIST(...) \
    template <> struct member_list<_reflecting_t> { typedef boost::mpl::vector<__VA_ARGS__> _member_list_t; }; \
/**/
#define YUKI_REFL_MEMBER_ACCESS_DECL(r, _, _member_signature) \
template struct member_access<decltype(&_reflecting_t::_member_signature), &_reflecting_t::_member_signature, member_access_tag<r>>; \
/**/
#define YUKI_REFL_MEMBER(r, _, _member) \
    ::yuki::reflection::detail::class_member<decltype(member_ptr(member_access_tag<r>())), member_ptr(member_access_tag<r>()), YUKI_MAKE_CHAR_LIST_STRINGIZE(_member)> \
/**/
#define YUKI_REFL_MEMBERS(...) \
    BOOST_PP_SEQ_FOR_EACH(YUKI_REFL_MEMBER_ACCESS_DECL, BOOST_PP_EMPTY(), BOOST_PP_TUPLE_TO_SEQ((__VA_ARGS__))) \
    YUKI_REFL_MEMBER_LIST(YUKI_TRANSFORM_INFIX_JOIN(BOOST_PP_COMMA, YUKI_REFL_MEMBER, BOOST_PP_EMPTY(), __VA_ARGS__)) \
/**/

#define YUKI_REFL_NESTED_TYPE_LIST(...) \
    template <> struct nested_type_list<_reflecting_t> { typedef boost::mpl::vector<__VA_ARGS__> _nested_type_list_t; }; \
/**/
#define YUKI_REFL_NESTED_TYPE(r, data, _identifier) \
    ::yuki::reflection::detail::nested_type<_reflecting_t::_identifier, YUKI_MAKE_CHAR_LIST_STRINGIZE(_identifier)> \
/**/
#define YUKI_REFL_NESTED_TYPES(...) \
    YUKI_REFL_NESTED_TYPE_LIST(YUKI_TRANSFORM_INFIX_JOIN(BOOST_PP_COMMA, YUKI_REFL_NESTED_TYPE, BOOST_PP_EMPTY(), __VA_ARGS__)) \
/**/
