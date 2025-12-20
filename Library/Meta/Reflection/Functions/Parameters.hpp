#pragma once

#include <Usagi/Library/Algorithms/Ranges.hpp>
#include <Usagi/Library/Meta/Reflection/Concepts.hpp>

namespace usagi::meta::reflection
{
template <std::meta::info Func>
    requires Function<Func>
consteval auto get_function_params()
{
    return std::meta::parameters_of(Func);
}

template <std::meta::info Func>
    requires FunctionOrFunctionType<Func>
consteval auto get_function_param_types()
{
    // https://eel.is/c++draft/meta.reflection#queries-62
    // consteval vector<info> parameters_of(info r);
    // Returns:
    // (62.1) If r represents a function F, then a vector containing reflections
    // of the parameters of F, in the order in which they appear in a
    // declaration of F.
    // (62.2) Otherwise, r represents a function type T; a
    // vector containing reflections of the types in parameter-type-list
    // ([dcl.fct]) of T, in the order in which they appear in the
    // parameter-type-list.
    if constexpr(std::meta::is_function(Func))
    {
        return std::meta::parameters_of(Func) |
            std::views::transform(std::meta::type_of);
    }
    else
    {
        return std::meta::parameters_of(Func);
    }
}

template <std::meta::info Refl>
concept HasExplicitThisParameter =
    (Function<Refl> || FunctionTemplate<Refl>) && [] {
        if constexpr(FunctionOrFunctionType<Refl>)
        {
            template for(constexpr auto p :
                std::define_static_array(get_function_params<Refl>()))
            {
                // `is_explicit_object_parameter` only works on reflection
                // of function parameters.
                if(std::meta::is_explicit_object_parameter(p)) return true;
                break;
            }
            return false;
        }
        else
        {
            // todo doesn't work on function templates
            static_assert(
                false, "currently cannot reflect function template parameters."
            );
        }
    }();

namespace static_tests
{
struct __s_param_refl_test
{
    void func(this __s_param_refl_test &) { }

    void func2(this auto &&) { }

    void func3() { }
};

consteval
{
    constexpr auto test_l   = [](int, bool, char = 'c') { };
    constexpr auto f_lambda = ^^decltype(test_l)::operator();

    constexpr std::array param_list_l { ^^int, ^^bool, ^^char };
    static_assert(
        ranges::is_equal(get_function_param_types<f_lambda>(), param_list_l)
    );
    static_assert(ranges::is_equal(
        get_function_param_types<std::meta::type_of(f_lambda)>(), param_list_l
    ));

    constexpr auto test_l_static   = [](...) static { };
    constexpr auto f_lambda_static = ^^decltype(test_l_static)::operator();

    constexpr std::array<std::meta::info, 0> param_list_l2 { };
    static_assert(ranges::is_equal(
        get_function_param_types<f_lambda_static>(), param_list_l2
    ));
    static_assert(ranges::is_equal(
        get_function_param_types<std::meta::type_of(f_lambda_static)>(),
        param_list_l2
    ));

    constexpr auto l3 = [](this auto &&) { };
    constexpr auto r3 = ^^decltype(l3)::operator()<decltype(l3)>;

    static_assert(!HasExplicitThisParameter<f_lambda>);
    static_assert(HasExplicitThisParameter<r3>);
    static_assert(HasExplicitThisParameter<^^__s_param_refl_test::func>);
    static_assert(HasExplicitThisParameter<
        ^^__s_param_refl_test::func2<__s_param_refl_test &>
    >);
    static_assert(!HasExplicitThisParameter<^^__s_param_refl_test::func3>);
}
} // namespace static_tests
} // namespace usagi::meta::reflection
