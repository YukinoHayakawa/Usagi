#pragma once

#include <Usagi/Library/Meta/Reflection/StaticReflection.hpp>

namespace usagi::meta::reflection
{
template <std::meta::info Refl>
concept FunctionOrFunctionType =
    std::meta::is_function(Refl) || std::meta::is_function_type(Refl);

struct function_parameter_info
{
    std::size_t num_total_parameters { };
    std::size_t num_required_parameters { };
};

template <std::meta::info Func>
    requires (std::meta::is_function(Func))
consteval function_parameter_info get_function_param_counts()
{
    function_parameter_info c;

    // Count how many params do NOT have defaults (must come first)
    for(auto param : std::define_static_array(std::meta::parameters_of(Func)))
    {
        ++c.num_required_parameters;
        ++c.num_total_parameters;
        if(std::meta::has_default_argument(param))
        {
            --c.num_required_parameters;
        }
    }

    return c;
}

namespace static_tests
{
consteval
{
    constexpr auto test_func = [](int, bool, char = 'c') { };
    constexpr auto num_params =
        get_function_param_counts<^^decltype(test_func)::operator()>();
    static_assert(num_params.num_total_parameters == 3);
    static_assert(num_params.num_required_parameters == 2);

    struct __s
    {
        void func(this __s &, int, ...);
    };

    constexpr auto f2_refl     = ^^__s::func;
    constexpr auto num_params2 = get_function_param_counts<f2_refl>();
    static_assert(num_params2.num_total_parameters == 2);
    static_assert(num_params2.num_required_parameters == 2);
    static_assert(std::meta::has_ellipsis_parameter(f2_refl));
    static_assert(std::meta::is_explicit_object_parameter(
        std::meta::parameters_of(f2_refl)[0]
    ));
}
} // namespace static_tests
} // namespace usagi::meta::reflection
