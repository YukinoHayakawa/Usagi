#pragma once

#include <Usagi/Preprocessor/make_nested_namespace.hpp>
#include <Usagi/Preprocessor/infix_join.hpp>
#include <Usagi/Preprocessor/op.hpp>

#define YUKI_REFL_IMPL_DEFINE_LIST(_namespaces, _identifier, list_type) \
    /* use YUKI_HEAD_UNPACK instaed of YUKI_TAIL_UNPACK, or MSVC will consider YUKI_OP_SCOPE YUKI_TAIL_UNPACK _namespaces as a single argument */ \
    typedef typename meta::YUKI_INFIX_JOIN(YUKI_OP_SCOPE, YUKI_HEAD_UNPACK _namespaces _identifier, list_type<_reflecting_t>::_##list_type##_t) _##list_type##_t \
/**/

/**
 * \brief Common code for reflecting classes and templates
 * \param _namespaces The namespace the class is in, wrapped in a tuple with each scope separated by commas.
 * \param _identifier The name of the class or template.
 * \param _elements Reflection information.
 * \param _macro_reflected_type_decl A macro of the form of M(_namespaces, _identifier), which defines _reflecting_t when invoked.
 * \param _macro_trait_class_decl A macro of the form of M(_namespaces, _identifier),
 * which generates the class header of information wrapper class when invoked.
 * \param _macro_reflected_type_nested_decl A macro of the form of M(_namespaces, _identifier),
 * which defines _reflecting_t inside the information wrapper class when invoked.
 */
#define YUKI_REFL_BASE(_namespaces, _identifier, _elements, _macro_reflected_type_decl, _macro_trait_class_decl, _macro_reflected_type_nested_decl) \
/* use a nested namespace to provide a scope for declaring a _reflecting_t for each class,
   avoiding passing the class name to each trait definition macro. */ \
namespace yuki { namespace reflection { namespace detail { namespace meta { YUKI_MAKE_NESTED_NAMESPACE((YUKI_HEAD_UNPACK _namespaces _identifier), (\
YUKI_USE_NAMESPACE(_namespaces) /* introduce identifiers from the namespace containing the class */ \
_macro_reflected_type_decl(_namespaces, _identifier); \
/* default traits of the class, which will be used if user does not define */ \
template <typename T> struct base_list { typedef boost::mpl::vector<> _base_list_t; }; \
template <typename T> struct member_list { typedef boost::mpl::vector<> _member_list_t; }; \
template <typename T> struct nested_type_list { typedef boost::mpl::vector<> _nested_type_list_t; }; \
/* use explicit template instantiation with friend functions to expose pointers to members, 
   including those to protected and private ones, then use tag classes to resolve the overload.
   for templates, however, this trick does not work since explicit template instantitation
   requires a complete argument list, which is not the case when declaring the member_list.
   and because it can only be declared in namespace scopes, we cannot use it in the actual
   trait class below, losing the last opportunity. therefore to reflect private members of
   a template, a friend declaration must be used.
   for the explicit instantiation trick, refer to:
   http://bloglitb.blogspot.jp/2011/12/access-to-private-members-safer.html */ \
template <typename MemberPtr, MemberPtr Ptr, typename AccessTag> struct member_access { friend constexpr auto member_ptr(AccessTag) { return Ptr; } }; \
template <size_t I> struct member_access_tag { }; \
/* template specializations, using _reflecting_t to refer to the class.
   each implementation may use different macros */ \
YUKI_UNPACK _elements \
)) }}}} /* namespace yuki::reflection::detail::meta */ \
namespace yuki { namespace reflection { namespace detail { \
_macro_trait_class_decl(_namespaces, _identifier) { \
private: \
    _macro_reflected_type_nested_decl(_namespaces, _identifier); \
    YUKI_REFL_IMPL_DEFINE_LIST(_namespaces, _identifier, base_list); \
    YUKI_REFL_IMPL_DEFINE_LIST(_namespaces, _identifier, member_list); \
    YUKI_REFL_IMPL_DEFINE_LIST(_namespaces, _identifier, nested_type_list); \
public: \
    static constexpr auto identifier = make_string_literal(#_identifier); \
    struct base_classes : base_classes_base<_reflecting_t, _base_list_t> { }; \
    struct class_members : class_members_base<_member_list_t> { }; \
    struct nested_types : nested_types_base<_nested_type_list_t> { }; \
}; \
}}} /* namespace yuki::reflection::detail */ \
/**/
