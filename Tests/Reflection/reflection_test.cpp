#include <type_traits>

#include <Usagi/Reflection/reflection.hpp>

namespace foo { namespace bar {

class A { };
class B { };

class C : public A, public virtual B
{
public:
    void foo() { }
    void bar() { }

    int baz;

    class D { };
    class E { };

    enum F { };
};

class D { };

}}

YUKI_REFL_CLASS((foo, bar), C, (
    YUKI_REFL_BASE_CLASSES(A, B)
    YUKI_REFL_NESTED_TYPES(D, E, F)
    YUKI_REFL_MEMBERS(foo, bar, baz)
))

YUKI_REFL_CLASS((foo, bar), D, (
))

using namespace yuki;
using namespace foo::bar;

// base class tests
static_assert(class_traits<C>::base_classes::size == 2, "base class size");

static_assert(std::is_same<class_traits<C>::base_classes::get<0>::type, A>::value, "base class 0 type");
static_assert(class_traits<C>::base_classes::get<0>::is_virtual == false, "base class 0 is_virtual");

static_assert(std::is_same<class_traits<C>::base_classes::get<1>::type, B>::value, "base class 1 type");
static_assert(class_traits<C>::base_classes::get<1>::is_virtual == true, "base class 1 is_virtual");

// member tests
static_assert(class_traits<C>::class_members::size == 3, "member size");

static_assert(class_traits<C>::class_members::get<0>::name == "foo", "member 0 name");
static_assert(class_traits<C>::class_members::get<0>::pointer == &C::foo, "member 0 pointer");

// nested type tests
static_assert(class_traits<C>::nested_types::size == 3, "nested type size");

static_assert(std::is_same<class_traits<C>::nested_types::get<0>::type, C::D>::value, "nested type 0 type");
static_assert(class_traits<C>::nested_types::get<0>::identifier == "D", "nested type 0 identifier");

static_assert(std::is_same<class_traits<C>::nested_types::get<2>::type, C::F>::value, "nested type 2 type");
static_assert(class_traits<C>::nested_types::get<2>::identifier == "F", "nested type 2 identifier");

// declaration in root namespace
class Root
{
public:
    class A { };
};

YUKI_REFL_CLASS((), Root, (
    YUKI_REFL_NESTED_TYPES(A)
))

template <typename T, typename U>
class TA { };
class TB { };
template <typename T>
class T1 { };

// template class tests
template <typename T, typename U, template <typename> typename TT, int I>
struct TC : TA<T, U>, virtual TB
{
    void foo() { }
    void bar() { }

    T baz;

    class D { };
    class E { };

    enum F { };
};

#define YUKI_REFL_T_PARAMS() \
((typename T, typename U, template <typename> typename TT, int I), (T, U, TT, I))
YUKI_REFL_TEMPLATE((), TC, (
    YUKI_REFL_T_BASE_CLASSES((TA<T,U>), (TB))
    YUKI_REFL_T_NESTED_TYPES(D, E, F)
    YUKI_REFL_T_MEMBERS(foo, bar, baz)
))
#undef YUKI_REFL_T_PARAMS

static_assert(std::is_same<class_traits<TC<int, bool, T1, 0>>::base_classes::get<0>::type, TA<int, bool>>::value, "template base class 0 type");
static_assert(class_traits<TC<int, bool, T1, 0>>::class_members::get<2>::pointer == &TC<int, bool, T1, 0>::baz, "template instance member pointer");
