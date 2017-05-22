#pragma once

/*
 * This file contains a set of macro for defining compile-time reflection information
 * for class and enum. The structure of definied information is conforming to std::class_traits
 * described in N4428 [1], with an extra field class_traits::identifier containing the
 * name of reflected class and nested in another namespace. The contents of the structures,
 * however, are not identical to that described in the proposal. The user of macro must
 * decide what information should be exposed, not only for public base classes and members.
 * 
 * Other macros for fine-grained control over exposed information will be added in the future
 * to support serialization, script binding, GUI widget rendering, etc.
 * 
 * To use the macros, follow the example below:
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
 * todo: support template classes
 * todo: simplify declarations
 * todo: allow reflecting private/protected members
 * todo: dynamic reflection
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
