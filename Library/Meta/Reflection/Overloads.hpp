#pragma once

#include <algorithm>
#include <ranges>

#include "StaticReflection.hpp"

namespace usagi::meta::reflection
{
// Refl must be a reflection of a function template specialization, and we
// will extract info from it to find its non-template opposite overload.
// It doesn't matter whether it's from a namespace or as a class member.
// todo: support constructors, conversion operators, different parameters.
template <std::meta::info Refl>
    requires (
        std::meta::is_function(Refl) && !std::meta::is_destructor(Refl) &&
        std::meta::has_template_arguments(Refl)
    )
struct overload_selector
{
    template <typename>
    struct scope_probe
    {
    };

    static consteval auto select_function_template_specialization()
    {
        return Refl;
    }

    static consteval auto select_non_template_function() -> std::meta::info
    {
        constexpr bool is_class_member = std::meta::is_class_member(Refl);
        constexpr auto parameter_types = std::define_static_array(
            std::meta::parameters_of(Refl) |
            std::views::transform(std::meta::type_of)
        );
        constexpr bool is_operator = std::meta::is_operator_function(Refl);
        constexpr bool has_identifier =
            std::meta::has_identifier(std::meta::template_of(Refl));
        constexpr auto ctx = is_class_member
            // instantiation-dependent visibility
            ? std::meta::access_context::current().via(
                  std::meta::parent_of(Refl)
              )
            // we need the same visibility as Refl so we build a probe which is
            // instantiated on-site. ... maybe this is meaningless.
            : std::meta::access_context::current().via(^^scope_probe<void>);
        // parent may be a type or a namespace. if it's a namespace, we
        // will have to iterate over all visible overload candidates. it is
        // basically impossible to do so at the moment without being able to
        // obtain either the function overload set or the set of namespaces
        // that may contain overloads. so we only deal with the same namespace
        // containing Refl.
        // otherwise, we only visit members within the same class.
        template for(constexpr auto m :
            std::define_static_array(
                std::meta::members_of(
                    is_class_member ? ctx.designating_class()
                                    : std::meta::parent_of(Refl),
                    ctx
                ) |
                std::views::filter(std::meta::is_function) /*|
                // can't filter for twice because the last filter cannot
                // guarantee to produce well-formed results
                // and we shouldn't drop special member functions.
                std::views::drop_while(
                    std::meta::is_special_member_function
                )*/
            ))
        {
            // note that conversion/operator/literal operators, ctors, dtor,
            // don't have identifiers.
            // conversion/literal operators have is_operator_function==false
            if constexpr(has_identifier && std::meta::has_identifier(m))
            {
                if(std::meta::identifier_of(m) !=
                    std::meta::identifier_of(std::meta::template_of(Refl)))
                    continue;
            }
            // note that assignment, as a special member function, is also
            // an operator function.
            if constexpr(is_operator && std::meta::is_operator_function(m))
            {
                if(std::meta::operator_of(m) != std::meta::operator_of(Refl))
                    continue;
            }
            else if constexpr(std::meta::is_literal_operator(m))
            {
                static_assert(
                    false,
                    "todo: currently we have no way to confirm a literal "
                    "operator's identity."
                );
            }
            if constexpr(std::meta::is_destructor(m))
            {
                continue;
            }
            // at this point, we have ruled out operators, dtor, and literal
            // operators. we have either confirmed the identifier or the
            // operator type. ctors and dtor don't have return type and
            // there is no dtor for us to overload.
            // but we also have to check the return type and parameter types of
            // an operator.
            else if constexpr(!std::meta::is_constructor(m))
            {
                if constexpr(
                    std::meta::is_same_type(
                        std::meta::return_type_of(m),
                        std::meta::return_type_of(Refl)
                    ))
                {
                    if(std::ranges::equal(
                           std::meta::parameters_of(m) |
                               std::views::transform(std::meta::type_of),
                           parameter_types
                       ))
                    {
                        return m;
                    }
                }
            }
        }
        return { };
    }
};

template <std::meta::info Refl>
constexpr std::meta::info select_function_template_specialization_refl_v =
    overload_selector<Refl>::select_function_template_specialization();

template <std::meta::info Refl>
constexpr std::meta::info select_non_template_function_overload_refl_v =
    overload_selector<Refl>::select_non_template_function();

/* doesn't work and somehow crashes the compiler at:
 * `clang::TreeTransform<(anonymous namespace)::TemplateInstantiator>::
 *     TransformCXXReflectExpr`
 * note that `remove_pointer` on member function pointer types has no effect.
 * once a member function id-expression is decayed to a pointer, there is no
 * way back.
 * so anything involving `static_cast<void (T::*)()>(&T::func)` won't work.
constexpr auto find_overload = []<typename T, typename Ret, typename... Args>(
                                   Ret (T::*func_ptr)(Args...)
                               ) -> std::meta::info {
    constexpr auto ctx = std::meta::access_context::current().via(^^T);
    typedef Ret (T::*mem_func_ptr_t)(Args...);
    // constexpr auto mem_func_t =
    // std::meta::remove_pointer(^^mem_func_ptr_t);
    template for(constexpr auto m :
        std::define_static_array(std::meta::members_of(^^T, ctx)))
    {
        if constexpr(
            std::meta::is_function(m) && ! std::meta::is_constructor(m) &&
            !std::meta::is_destructor(m))
        {
            if constexpr(
                std::meta::is_same_type(
                    std::meta::add_pointer(std::meta::type_of(m)),
                    ^^mem_func_ptr_t
                ))
            {
                return m;
            }
        }
    }
    return {};
};
*/

namespace static_tests
{
struct __overload_selection_test
{
    __overload_selection_test(char, int) { }

    template <typename...>
    __overload_selection_test()
    {
    }

    static void operator()(int, bool) { }

    static_assert(!std::meta::has_identifier(^^operator()));

    template <typename>
    static void operator()()
    {
    }

    template <typename... Args>
    static void operator()(Args...)
    {
    }

    static_assert(std::meta::is_operator_function(^^operator()<>));

    void operator++() { }

    template <typename>
    void operator++()
    {
    }

    template <typename...>
    void operator++()
    {
    }

    constexpr int func(int) { return 0; }

    static_assert(std::meta::has_identifier(^^func));

    template <typename... Args>
    constexpr int func(Args...)
    {
        return 1;
    }

    static_assert(std::meta::has_identifier(std::meta::template_of(^^func<>)));

    operator bool() { return false; }

    template <typename T>
    operator T()
    {
        return T { };
    }
};

void __overload_selection_test_func()
{
}

template <typename...>
void __overload_selection_test_func()
{
}

// confirm that we can use member function pointer as constexpr
static_assert(
    static_cast<void (__overload_selection_test::*)()>(
        &__overload_selection_test::operator++
    ) ==
    static_cast<void (__overload_selection_test::*)()>(
        &__overload_selection_test::operator++
    )
);

// true so long as it is a specialization even there is zero arguments.
// this is how we can tell a function specialization apart from an ordinary
// function.
static_assert(
    std::meta::has_template_arguments(^^__overload_selection_test::operator()<>)
);

// free function

static_assert(std::meta::is_function(
    select_function_template_specialization_refl_v<
        ^^__overload_selection_test_func<>
    >
));

static_assert(
    select_function_template_specialization_refl_v<
        ^^__overload_selection_test_func<>
    > == ^^__overload_selection_test_func<>
);

static_assert(std::meta::is_function(
    select_non_template_function_overload_refl_v<
        ^^__overload_selection_test_func<>
    >
));

consteval
{
    constexpr auto refl = select_non_template_function_overload_refl_v<
        ^^__overload_selection_test_func<>
    >;
    static_assert(&[:refl:] ==
            static_cast<void (*)()>(&__overload_selection_test_func),
        std::meta::display_string_of(refl));
}

// non-static member function

static_assert(std::meta::is_function(
    select_function_template_specialization_refl_v<
        ^^__overload_selection_test::func<int>
    >
));

static_assert(
    select_function_template_specialization_refl_v<
        ^^__overload_selection_test::func<int>
    > == ^^__overload_selection_test::func<int>
);

static_assert(std::meta::is_function(
    select_non_template_function_overload_refl_v<
        ^^__overload_selection_test::func<int>
    >
));

consteval
{
    constexpr auto refl = select_non_template_function_overload_refl_v<
        ^^__overload_selection_test::func<int>
    >;
    static_assert(&[:refl:] ==
            static_cast<int (__overload_selection_test::*)(int)>(
                &__overload_selection_test::func
            ),
        std::meta::display_string_of(refl));
}

// non-static member operators

static_assert(std::meta::is_function(
    select_function_template_specialization_refl_v<
        ^^__overload_selection_test::operator++ <>
    >
));

static_assert(
    select_function_template_specialization_refl_v<
        ^^__overload_selection_test::operator++ <>
    > == ^^__overload_selection_test::operator++ <>
);

static_assert(std::meta::is_function(
    select_non_template_function_overload_refl_v<
        ^^__overload_selection_test::operator++ <>
    >
));

consteval
{
    constexpr auto refl = select_non_template_function_overload_refl_v<
        ^^__overload_selection_test::operator++ <>
    >;
    static_assert(&[:refl:] ==
            static_cast<void (__overload_selection_test::*)()>(
                &__overload_selection_test::operator++
            ),
        std::meta::display_string_of(refl));
}

// non-static conversion operators

/*
 * 13.7.3 [temp.mem] Member templates
 * Clarify in Note 1 that a specialization of a conversion function template can
 * be formed through a splice-expression.
 * But...
 * An expression designating a particular specialization of a conversion
 * function template can only be formed with a splice-expression. There is no
 * analogous syntax to form a template-id ([temp.names]) for such a function by
 * providing an explicit template argument list ([temp.arg.explicit]). — end
 * note ]
 * This means it's only possible to obtain the reflection of a conversion
 * function template and splice it to instantiate it. This requires going over
 * the class's members to locate the conversion function template first.
 */

/* invalid syntax
static_assert(std::meta::is_function(
    select_function_template_specialization_refl_v<
        ^^__overload_selection_test::operator bool<bool>
    >
));

static_assert(
    select_function_template_specialization_refl_v<
        ^^__overload_selection_test::operator bool<bool>
    > == ^^__overload_selection_test::operator bool<bool>
);

static_assert(std::meta::is_function(
    select_non_template_function_overload_refl_v<
        ^^__overload_selection_test::operator bool<bool>>
));

consteval
{
    constexpr auto refl = select_non_template_function_overload_refl_v<
        ^^__overload_selection_test::operator bool<bool>
    >;
    static_assert(&[:refl:] ==
        static_cast<bool (__overload_selection_test::*)()>(
            &__overload_selection_test::operator bool
        ),
        std::meta::display_string_of(refl)
    );
}
*/

// static member operators

static_assert(std::meta::is_function(
    select_function_template_specialization_refl_v<
        ^^__overload_selection_test::operator()<int, bool>
    >
));

static_assert(
    select_function_template_specialization_refl_v<
        ^^__overload_selection_test::operator()<int, bool>
    > == ^^__overload_selection_test::operator()<int, bool>
);

static_assert(std::meta::is_function(
    select_non_template_function_overload_refl_v<
        ^^__overload_selection_test::operator()<int, bool>
    >
));

consteval
{
    constexpr auto refl = select_non_template_function_overload_refl_v<
        ^^__overload_selection_test::operator()<int, bool>
    >;
    static_assert(&[:refl:] ==
            static_cast<void (*)(int, bool)>(
                &__overload_selection_test::operator()
            ),
        std::meta::display_string_of(refl));
}

// constructors

/*
 * https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2025/p2996r13.html#splicing-reflections-of-constructors
 * todo: The current proposal explicitly forbids splicing constructors, and C++
 *   forbids taking address of a constructor, so well, just give up.
 *
 * 4.2.2.1 Splicing reflections of constructors
 * Iterating over the members of a class (e.g., using std::meta::members_of)
 * allows one, for the first time, to obtain “handles” representing
 * constructors. An immediate question arises of whether it’s possible to reify
 * these constructors to construct objects, or even to take their address. While
 * we are very interested in exploring these ideas, we defer their discussion to
 * a future paper; this proposal disallows splicing a reflection of a
 * constructor (or constructor template) in any context.
 */

/*
static_assert(std::meta::is_function(
    select_function_template_specialization_refl_v<
        ^^__overload_selection_test::__overload_selection_test<>
    >
));

static_assert(
    select_function_template_specialization_refl_v<std::meta::reflect_function(
        ^^__overload_selection_test::__overload_selection_test<>
    )> ==
    std::meta::reflect_function(
        ^^__overload_selection_test::__overload_selection_test<>
    )
);

static_assert(std::meta::is_function(
    select_non_template_function_overload_refl_v<std::meta::reflect_function(
        ^^__overload_selection_test::__overload_selection_test<>
    )>
));
*/
} // namespace static_tests
} // namespace usagi::meta::reflection
