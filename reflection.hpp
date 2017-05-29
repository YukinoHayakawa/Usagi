#pragma once

/*
 * This file contains a set of macro for defining compile-time reflection information
 * for classes and templates. The structure of definied information is conforming to std::class_traits
 * described in N4428 [1], with an extra field class_traits::identifier containing the
 * name of reflected class. The contents of the structures, however, are not identical to
 * that described in the proposal. The user of macro must decide what information should be
 * exposed.
 * 
 * Other macros for fine-grained control over exposed information may be implemented on
 * top of the class_traits<> instances,  to support serialization, script binding, GUI widget
 * rendering, etc.
 * 
 * Note that, before reflecting nested template types is supported, it's not recommended
 * to reflect nested types, since the interface is subject to changes caused by that
 * function.
 * 
 * todo: simplify declarations
 * todo: allow reflecting private/protected members
 * todo: dynamic reflection
 * todo: nested template types
 * todo: overloaded members
 * 
 * [1] http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2015/n4428.pdf
 */

#include "detail/simple_class_traits.hpp"
#include "detail/template_class_traits.hpp"

namespace yuki
{

template <typename T>
struct class_traits
    : reflection::detail::simple_class_traits<T>
    , reflection::detail::template_class_traits<T>
{
};

}
