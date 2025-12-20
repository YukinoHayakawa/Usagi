#pragma once

#include <Usagi/Library/Meta/Reflection/Concepts.hpp>

namespace usagi::meta::reflection
{
// todo: there are some issues regarding `members_of` which won't return
//   functions whose declared type involves `auto` placeholder. this involves
//   a non-generic lambda's `operator()` and functions like `auto func() {}`
//   (note that no trailing return type is written here.)
//   `auto func() -> void { }` works fine though.
//   It seems the reason is `Sema::isReflectableDecl` is using a function's
//   declared type to decide whether a function should appear in the results of
//   `members_of`.
//   https://github.com/bloomberg/clang-p2996/issues/218
//   https://github.com/bloomberg/clang-p2996/issues/219
//   https://github.com/bloomberg/clang-p2996/issues/171
//   https://github.com/bloomberg/clang-p2996/issues/179
// todo: allow picking overloads by parameter types.
template <
    std::meta::info      Refl,
    std::meta::operators Op,
    bool                 IsTemplate = false
>
consteval auto find_operator_in_class() -> std::meta::info
    requires (std::meta::is_class_type(Refl))
{
    constexpr auto ctx = std::meta::access_context::current().via(Refl);
    template for(constexpr auto m :
        std::define_static_array(std::meta::members_of(Refl, ctx)))
    {
        if constexpr(OperatorFunctionOrTemplateOf<m, Op>)
        {
            // static_assert(false, std::meta::display_string_of(m));
            if constexpr(IsTemplate)
            {
                if(std::meta::is_operator_function_template(m)) return m;
            }
            else
            {
                if(std::meta::is_operator_function(m)) return m;
            }
        }
    }
    return { };
}

namespace static_tests
{
constexpr auto operator""_optest(const unsigned long long val)
{
    return val;
}

template <char...>
constexpr auto operator""_optest2()
{
    return 1;
}

struct __operators_test_1
{
    auto operator()() { }

    template <typename...>
    void operator()()
    {
    }
};

struct __operators_test_2
{
    void operator()(auto...) { }
};

consteval
{
    constexpr auto test = []<std::meta::info       Refl,
                              std::meta::operators Op,
                              std::meta::operators NotOp,
                              bool                 IsTemplate> consteval {
        static_assert(OperatorFunctionOrTemplate<Refl>);
        static_assert(OperatorFunctionOrTemplateOf<Refl, Op>);
        static_assert(!OperatorFunctionOrTemplateOf<Refl, NotOp>);
        if constexpr(IsTemplate)
            static_assert(std::meta::is_operator_function_template(Refl));
        else
            static_assert(std::meta::is_operator_function(Refl));
        return true;
    };

    // lambdas
    // todo bug https://github.com/bloomberg/clang-p2996/issues/218
    //   lambda return type not deduced causing `operator()` not included in
    //   the results of `members_of`.
    constexpr static int i   = 1;
    // constexpr auto l   = [] -> void { };
    constexpr auto       l   = [] { };
    // constexpr auto       l   = []<typename...> -> void { };
    constexpr auto       l2  = [](int) { };
    // constexpr auto       l2  = []<typename...>(int) { };
    constexpr auto       lc  = [&] { return i; };
    // constexpr auto       lc  = [&]<typename...> { return i; };
    constexpr auto       lt  = []<typename, typename...> { };
    constexpr auto       lt2 = []<typename...> { };
    constexpr auto       lt3 = [](auto...) { };

    static_assert(test.operator()<
        ^^decltype(l)::operator(),
        std::meta::operators::op_parentheses,
        std::meta::operators::op_caret,
        false
    >());
    static_assert(test.operator()<
        ^^decltype(lt)::operator(),
        std::meta::operators::op_parentheses,
        std::meta::operators::op_caret,
        true
    >());
    static_assert(test.operator()<
        ^^decltype(lt)::operator()<int, int>,
        std::meta::operators::op_parentheses,
        std::meta::operators::op_caret,
        false
    >());

    static_assert(
        !std::meta::is_literal_operator_template(^^operator""_optest) &&
        std::meta::is_literal_operator(^^operator""_optest)
    );
    static_assert(
        std::meta::is_literal_operator_template(^^operator""_optest2) &&
        !std::meta::is_literal_operator(^^operator""_optest2)
    );

    static_assert(
        find_operator_in_class<
            ^^__operators_test_1,
            std::meta::operators::op_parentheses,
            false
        >() != std::meta::info { }
    );
    static_assert(
        find_operator_in_class<
            ^^__operators_test_1,
            std::meta::operators::op_parentheses,
            true
        >() != std::meta::info { }
    );
    static_assert(
        find_operator_in_class<
            ^^__operators_test_2,
            std::meta::operators::op_parentheses,
            true
        >() == ^^__operators_test_2::operator()
    );
    static_assert(
        find_operator_in_class<
            ^^decltype(l),
            std::meta::operators::op_parentheses,
            false
        >() == ^^decltype(l)::operator()
    );
    static_assert(
        find_operator_in_class<
            ^^decltype(l2),
            std::meta::operators::op_parentheses,
            false
        >() == ^^decltype(l2)::operator()
    );
    static_assert(
        find_operator_in_class<
            ^^decltype(lc),
            std::meta::operators::op_parentheses,
            false
        >() == ^^decltype(lc)::operator()
    );
    static_assert(
        find_operator_in_class<
            ^^decltype(lt),
            std::meta::operators::op_parentheses,
            true
        >() == ^^decltype(lt)::operator()
    );
    static_assert(
        find_operator_in_class<
            ^^decltype(lt2),
            std::meta::operators::op_parentheses,
            true
        >() == ^^decltype(lt2)::operator()
    );
    static_assert(
        find_operator_in_class<
            ^^decltype(lt3),
            std::meta::operators::op_parentheses,
            true
        >() == ^^decltype(lt3)::operator()
    );
}
} // namespace static_tests
} // namespace usagi::meta::reflection
