#pragma once

#include <boost/mpl/vector.hpp>
#include <boost/mpl/size.hpp>
#include <boost/mpl/at.hpp>
#include <boost/type_traits/is_virtual_base_of.hpp>

namespace yuki
{
namespace reflection
{
namespace detail
{

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
