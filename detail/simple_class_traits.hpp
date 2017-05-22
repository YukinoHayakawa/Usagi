#pragma once

#include <boost/mpl/vector.hpp>
#include <boost/mpl/size.hpp>
#include <boost/mpl/at.hpp>
#include <boost/type_traits/is_virtual_base_of.hpp>

#include <Usagi/Meta/char_list.hpp>
#include <Usagi/Preprocessor/infix_join.hpp>
#include <Usagi/Preprocessor/make_nested_namespace.hpp>
#include <Usagi/Preprocessor/identifier.hpp>
#include <Usagi/Preprocessor/op.hpp>

namespace yuki
{
namespace reflection
{
namespace detail
{

template <typename>
struct simple_class_traits;

// todo: (C++17) use auto for Ptr
template <typename P, P Ptr, typename NameCharList>
struct class_member
{
    typedef NameCharList MemberName;
    static constexpr P member_pointer = Ptr;
};

template <typename T, typename IdentifierCharList>
struct nested_type
{
    typedef IdentifierCharList Identifier;
    typedef T nested_type_t;
};
//
//template <typename T, typename IdentifierCharList>
//struct nested_template
//{
//    typedef IdentifierCharList Identifier;
//    template <typename... Args> using nested_template_t = T<Args...>;
//};

template <typename Reflected, typename BaseClasses>
struct base_classes_base
{
    template <size_t Index> struct get
    {
        typedef typename boost::mpl::at<BaseClasses, boost::mpl::int_<Index>>::type type;
        static constexpr bool is_virtual = boost::is_virtual_base_of<type, Reflected>::value;
    };
    static constexpr size_t size = boost::mpl::size<BaseClasses>::type::value;
};

template <typename Members>
struct class_members_base
{
    template <size_t Index> struct get
    {
        static constexpr auto name = boost::mpl::at<Members, boost::mpl::int_<Index>>::type::MemberName::to_string_literal();
        static constexpr auto pointer = boost::mpl::at<Members, boost::mpl::int_<Index>>::type::member_pointer;
    };
    static constexpr size_t size = boost::mpl::size<Members>::type::value;
};

template <typename NestedTypes>
struct nested_types_base
{
    template <size_t Index>
    struct get
    {
        static constexpr auto identifier = boost::mpl::at<NestedTypes, boost::mpl::int_<Index>>::type::Identifier::to_string_literal();
        typedef typename boost::mpl::at<NestedTypes, boost::mpl::int_<Index>>::type::nested_type_t type;
    };
    static constexpr size_t size = boost::mpl::size<NestedTypes>::type::value;
};

}
}
}

/**
* \brief Main macro for defining class reflection information. Invoke this macro in the
* root namespace then access reflection info through yuki::class_traits<T>.
* \param _namespace The namespace in which the class is declared, separated into unqualified
* identifiers and surrounded by a pair of parentheses.
* \param _identifier The unqualified name of reflected class.
* \param _elements Optional elements enclosed in a pair of (). Available elements include
* YUKI_REFL_BASE_CLASSES, YUKI_REFL_NESTED_TYPES, and YUKI_REFL_MEMBERS.
*/
#define YUKI_REFL_CLASS(_namespaces, _identifier, _elements) \
namespace yuki { namespace reflection { namespace detail { namespace meta { YUKI_MAKE_NESTED_NAMESPACE(_namespaces, (\
YUKI_USE_NAMESPACE(_namespaces) /* introduce identifiers from the namespace containing the class */ \
typedef YUKI_CANONICAL_CLASS_ID(_namespaces, _identifier) _reflecting_t; \
template <typename T> struct base_list { typedef boost::mpl::vector<> _base_list_t; }; \
template <typename T> struct member_list { typedef boost::mpl::vector<> _member_list_t; }; \
template <typename T> struct nested_list { typedef boost::mpl::vector<> _nested_type_list_t; }; \
YUKI_UNPACK _elements /* template specializations, they use _reflecting_t */ \
)) }}}} /* namespace yuki::reflection::detail::meta */ \
namespace yuki { namespace reflection { namespace detail { \
template <> struct simple_class_traits<YUKI_CANONICAL_CLASS_ID(_namespaces, _identifier)> { \
private: \
    typedef YUKI_CANONICAL_CLASS_ID(_namespaces, _identifier) _reflecting_t; \
    /* use YUKI_HEAD_UNPACK instaed of YUKI_TAIL_UNPACK, or MSVC will consider YUKI_OP_SCOPE YUKI_TAIL_UNPACK _namespaces as a single argument */ \
    typedef typename meta::YUKI_INFIX_JOIN(YUKI_OP_SCOPE, YUKI_HEAD_UNPACK _namespaces base_list<_reflecting_t>::_base_list_t) _base_list_t; \
    typedef typename meta::YUKI_INFIX_JOIN(YUKI_OP_SCOPE, YUKI_HEAD_UNPACK _namespaces member_list<_reflecting_t>::_member_list_t) _member_list_t; \
    typedef typename meta::YUKI_INFIX_JOIN(YUKI_OP_SCOPE, YUKI_HEAD_UNPACK _namespaces nested_list<_reflecting_t>::_nested_type_list_t) _nested_type_list_t; \
public: \
    static constexpr auto identifier = make_string_literal(#_identifier); \
    struct base_classes : base_classes_base<_reflecting_t, _base_list_t> { }; \
    struct class_members : class_members_base<_member_list_t> { }; \
    struct nested_types : nested_types_base<_nested_type_list_t> { }; \
}; \
}}} /* namespace yuki::reflection::detail */ \
/**/

// specialization of default traits

#define YUKI_REFL_BASE_CLASSES(...) \
    template <> struct base_list<_reflecting_t> { typedef boost::mpl::vector<__VA_ARGS__> _base_list_t; }; \
/**/

#define YUKI_REFL_MEMBER_LIST(...) \
    template <> struct member_list<_reflecting_t> { typedef boost::mpl::vector<__VA_ARGS__> _member_list_t; }; \
/**/
#define YUKI_REFL_MEMBER(_member) \
    ::yuki::reflection::detail::class_member<decltype(&_reflecting_t::_member), &_reflecting_t::_member, YUKI_MAKE_CHAR_LIST_STRINGIZE(_member)> \
/**/
#define YUKI_REFL_MEMBERS(...) \
    YUKI_REFL_MEMBER_LIST(YUKI_TRANSFORM_INFIX_JOIN(BOOST_PP_COMMA, YUKI_REFL_MEMBER, __VA_ARGS__)) \
/**/

#define YUKI_REFL_NESTED_TYPE_LIST(...) \
    template <> struct nested_list<_reflecting_t> { typedef boost::mpl::vector<__VA_ARGS__> _nested_type_list_t; }; \
/**/
#define YUKI_REFL_NESTED_TYPE(_identifier) \
    ::yuki::reflection::detail::nested_type<_reflecting_t::_identifier, YUKI_MAKE_CHAR_LIST_STRINGIZE(_identifier)> \
/**/
#define YUKI_REFL_NESTED_TYPES(...) \
    YUKI_REFL_NESTED_TYPE_LIST(YUKI_TRANSFORM_INFIX_JOIN(BOOST_PP_COMMA, YUKI_REFL_NESTED_TYPE, __VA_ARGS__)) \
/**/
