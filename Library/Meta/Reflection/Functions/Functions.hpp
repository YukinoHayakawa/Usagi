#pragma once

#include <Usagi/Library/Meta/Reflection/Closures.hpp>

namespace usagi::meta::reflection
{
struct function_meta_info
{
    std::meta::info function_refl;

    // number of total params.
    // `=parameters_of(func).size()`
    // note that, explicit `this` is counted while an implicit one is not.
    // also that `...` is not counted here.
    std::uint8_t num_total_params  = 0;
    // number of arguments required to call the function.
    // `=num_total_params-num_params_with_defaults()`
    // note that, explicit `this` is counted, and that if there is an `...`,
    // this would essentially equal to `num_total_params`.
    std::uint8_t num_required_args = 0;

    // if the function is a non-capturing lambda's `operator()`, it can be
    // used like a free function.
    bool is_non_capturing_lambda    : 1 = false;
    // true if the function is a template one, no matter whether the template
    // parameters are explicit or implicit (e.g. `auto` params, including
    // `this auto &&`.)
    bool is_template_specialization : 1 = false;
    // true if it is a non-static member function, which means it needs a `this`
    // pointer to be called.
    bool is_non_static_member       : 1 = false;
    // true if `this auto && self` or `this X& self`
    // note that a closure can auto have `this auto &&`
    // https://eel.is/c++draft/expr.prim.lambda.closure#6.3
    bool has_explicit_this          : 1 = false;
    // true if ends with `...` C-style va-args
    bool has_ellipsis_param         : 1 = false;
};

template <std::meta::info Func>
// The reflected entity must be a function. Otherwise, it would be almost
// useless in game engine contexts.
    requires (std::meta::is_function(Func))
consteval function_meta_info reflect_function()
{
    function_meta_info info;

    info.function_refl              = Func;
    info.is_template_specialization = std::meta::has_template_arguments(Func);
    info.is_non_static_member =
        std::meta::is_class_member(Func) && !std::meta::is_static_member(Func);
    info.has_ellipsis_param = std::meta::has_ellipsis_parameter(Func);

    if constexpr(
        std::meta::is_class_member(Func) &&
        ClosureType<std::meta::parent_of(Func)>)
    {
        constexpr auto outer_t = get_closure_type_from_call_operator<Func>();
        info.is_non_capturing_lambda = is_non_capturing_closure<outer_t>();
    }

    template for(constexpr auto p :
        std::define_static_array(std::meta::parameters_of(Func)))
    {
        ++info.num_total_params;
        ++info.num_required_args;
        if constexpr(std::meta::is_explicit_object_parameter(p))
            info.has_explicit_this = true;
        if constexpr(std::meta::has_default_argument(p))
            --info.num_required_args;
    }

    return info;
}

namespace static_tests
{
struct __s_func_refl_test
{
    void func(this __s_func_refl_test &, int, ...) { }

    void func2(this auto &&, int, ...) { }

    static void func3() { }
};

consteval
{
    constexpr auto test_l   = [](int, bool, char = 'c') { };
    constexpr auto f_lambda = ^^decltype(test_l)::operator();
    constexpr auto info     = reflect_function<f_lambda>();
    static_assert(info.function_refl == f_lambda);
    static_assert(info.num_total_params == 3);
    static_assert(info.num_required_args == 2);
    static_assert(!info.is_template_specialization);
    // non-capturing `closure::operator()` is non-static by default.
    static_assert(info.is_non_static_member);
    static_assert(!info.has_explicit_this);
    static_assert(!info.has_ellipsis_param);
    static_assert(info.is_non_capturing_lambda);

    constexpr auto test_l_static   = [](...) static { };
    constexpr auto f_lambda_static = ^^decltype(test_l_static)::operator();
    constexpr auto info_s          = reflect_function<f_lambda_static>();
    static_assert(info_s.function_refl == f_lambda_static);
    static_assert(info_s.num_total_params == 0);
    static_assert(info_s.num_required_args == 0);
    static_assert(!info_s.is_template_specialization);
    // but we can make it static
    static_assert(!info_s.is_non_static_member);
    static_assert(!info_s.has_explicit_this);
    static_assert(info_s.has_ellipsis_param);
    static_assert(info_s.is_non_capturing_lambda);

    constexpr auto i                = 1;
    constexpr auto test_l_capture   = [=] { return i; };
    constexpr auto f_lambda_capture = ^^decltype(test_l_capture)::operator();
    constexpr auto info_c           = reflect_function<f_lambda_capture>();
    static_assert(info_c.function_refl == f_lambda_capture);
    static_assert(info_c.num_total_params == 0);
    static_assert(info_c.num_required_args == 0);
    static_assert(!info_c.is_template_specialization);
    static_assert(info_c.is_non_static_member);
    static_assert(!info_c.has_explicit_this);
    static_assert(!info_c.has_ellipsis_param);
    static_assert(!info_c.is_non_capturing_lambda);

    constexpr auto test_l_capture_empty = [&] { };
    constexpr auto f_lambda_capture_empty =
        ^^decltype(test_l_capture_empty)::operator();
    constexpr auto info_ce = reflect_function<f_lambda_capture_empty>();
    static_assert(info_ce.function_refl == f_lambda_capture_empty);
    static_assert(info_ce.num_total_params == 0);
    static_assert(info_ce.num_required_args == 0);
    static_assert(!info_ce.is_template_specialization);
    static_assert(info_ce.is_non_static_member);
    static_assert(!info_ce.has_explicit_this);
    static_assert(!info_ce.has_ellipsis_param);
    static_assert(!info_ce.is_non_capturing_lambda);

    constexpr auto f_refl  = ^^__s_func_refl_test::func;
    constexpr auto f2_refl = ^^__s_func_refl_test::func2<__s_func_refl_test &>;
    constexpr auto f3_refl = ^^__s_func_refl_test::func3;

    constexpr auto info_f  = reflect_function<f_refl>();
    constexpr auto info_f2 = reflect_function<f2_refl>();
    constexpr auto info_f3 = reflect_function<f3_refl>();

    static_assert(info_f.function_refl == f_refl);
    static_assert(info_f.num_total_params == 2);
    static_assert(info_f.num_required_args == 2);
    static_assert(!info_f.is_template_specialization);
    static_assert(info_f.is_non_static_member);
    static_assert(info_f.has_explicit_this);
    static_assert(info_f.has_ellipsis_param);
    static_assert(!info_f.is_non_capturing_lambda);

    static_assert(info_f2.function_refl == f2_refl);
    static_assert(info_f2.num_total_params == 2);
    static_assert(info_f2.num_required_args == 2);
    static_assert(info_f2.is_template_specialization);
    static_assert(info_f2.is_non_static_member);
    static_assert(info_f2.has_explicit_this);
    static_assert(info_f2.has_ellipsis_param);
    static_assert(!info_f2.is_non_capturing_lambda);

    static_assert(info_f3.function_refl == f3_refl);
    static_assert(info_f3.num_total_params == 0);
    static_assert(info_f3.num_required_args == 0);
    static_assert(!info_f3.is_template_specialization);
    static_assert(!info_f3.is_non_static_member);
    static_assert(!info_f3.has_explicit_this);
    static_assert(!info_f3.has_ellipsis_param);
    static_assert(!info_f3.is_non_capturing_lambda);
}
} // namespace static_tests
} // namespace usagi::meta::reflection
