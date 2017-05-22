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
 * \param _macro_make_type A macro of the form of M(_namespaces, _identifier), which defines _reflecting_t when invoked.
 * \param _macro_make_class_decl A macro of the form of M(_namespaces, _identifier),
 * which generates the class header of information wrapper class when invoked.
 * \param _macro_make_class_type A macro of the form of M(_namespaces, _identifier),
 * which defines _reflecting_t inside the information wrapper class when invoked.
 */
#define YUKI_REFL_BASE(_namespaces, _identifier, _elements, _macro_make_type, _macro_make_class_decl, _macro_make_class_type) \
namespace yuki { namespace reflection { namespace detail { namespace meta { YUKI_MAKE_NESTED_NAMESPACE((YUKI_HEAD_UNPACK _namespaces _identifier), (\
YUKI_USE_NAMESPACE(_namespaces) /* introduce identifiers from the namespace containing the class */ \
_macro_make_type(_namespaces, _identifier); \
/* default traits, if user does not use the corresponding defining macros, these will be used */ \
template <typename T> struct base_list { typedef boost::mpl::vector<> _base_list_t; }; \
template <typename T> struct member_list { typedef boost::mpl::vector<> _member_list_t; }; \
template <typename T> struct nested_type_list { typedef boost::mpl::vector<> _nested_type_list_t; }; \
YUKI_UNPACK _elements /* template specializations, inside which only _reflecting_t is used */ \
)) }}}} /* namespace yuki::reflection::detail::meta */ \
namespace yuki { namespace reflection { namespace detail { \
_macro_make_class_decl(_namespaces, _identifier) { \
private: \
    _macro_make_class_type(_namespaces, _identifier); \
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
