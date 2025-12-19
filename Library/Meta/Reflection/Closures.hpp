#pragma once

#include "StaticReflection.hpp"

namespace usagi::meta::reflection
{
// https://en.cppreference.com/w/cpp/language/lambda.html
// The lambda expression is a prvalue expression of unique
// unnamed non-union non-aggregate class type, known as closure type.

// This concept only tests whether a given **type** satisfies the requirements
// as a closure type. Given a reflection of a lambda's identifier, a reference,
// or other things, use other corresponding facilities.
template <std::meta::info Refl>
concept ClosureType = std::meta::is_type(Refl) &&
    !std::meta::is_union_type(Refl) && !std::meta::is_aggregate_type(Refl) &&
    std::meta::is_class_type(Refl) && !std::meta::has_identifier(Refl);

template <std::meta::info Refl>
concept ClosureVariable =
    std::meta::is_variable(Refl) && ClosureType<std::meta::type_of(Refl)>;

template <std::meta::info Refl>
concept ClosureReferenceType =
    std::meta::is_type(Refl) && std::meta::is_reference_type(Refl) &&
    ClosureType<std::meta::remove_reference(Refl)>;

template <std::meta::info Refl>
concept ClosureReference = ClosureReferenceType<std::meta::type_of(Refl)>;

template <std::meta::info Refl>
concept ClosureObject =
    std::meta::is_object(Refl) && ClosureType<std::meta::type_of(Refl)>;

template <std::meta::info Refl>
concept CanExtractClosureType = ClosureType<Refl> || ClosureVariable<Refl> ||
    ClosureReferenceType<Refl> || ClosureReference<Refl> || ClosureObject<Refl>;

template <std::meta::info Refl>
    requires CanExtractClosureType<Refl>
consteval auto extract_closure_type_refl()
{
    if constexpr(ClosureType<Refl>)
        return Refl;
    else if constexpr(ClosureVariable<Refl>)
        return std::meta::type_of(Refl);
    else if constexpr(ClosureReferenceType<Refl>)
        return std::meta::remove_reference(Refl);
    // ClosureReference/ClosureObject
    return std::meta::remove_reference(std::meta::type_of(Refl));
}

// todo: there is a bug in the current clang fork causing it impossible to get
//   the invoke operator of a non-generic lambda using `find_operator_in_class`
//   https://github.com/bloomberg/clang-p2996/issues/219
template <std::meta::info Refl>
    requires CanExtractClosureType<Refl>
consteval auto extract_closure_invoke_operator_refl()
{
    // but this is more straightforward as a closure type can only have exactly
    // one `operator()`.
    constexpr auto closure_type_refl = extract_closure_type_refl<Refl>();
    using closure_t                  = typename[:closure_type_refl:];
    // this might be a function or function template, but a closure type can
    // only have exactly one of it so it must be callable.
    return ^^closure_t::operator();
}

namespace static_tests
{
consteval
{
    constexpr static auto a_lambda = [&](int) { };

    struct a_class
    {
    };

    constexpr auto ret_lv_ref = []<typename T>(T && var) -> decltype(auto) {
        return std::forward<T>(var);
    };

    constexpr auto refl_var   = ^^a_lambda;
    constexpr auto refl_type  = ^^decltype(a_lambda);
    constexpr auto refl_class = ^^a_class;

    // decltype of id-expression
    static_assert(ClosureType<refl_type>);
    // a named class
    static_assert(!ClosureType<refl_class>);
    // a variable
    static_assert(!ClosureType<refl_var>);
    static_assert(std::meta::is_variable(refl_var));
    // a lambda expression
    static_assert(ClosureType<^^decltype([] { })>);

    constexpr auto & lambda_lv_ref = ret_lv_ref(a_lambda);
    [[maybe_unused]]
    constexpr auto && lambda_rv_ref = std::move(lambda_lv_ref);

    constexpr auto refl_lv_ref      = ^^lambda_lv_ref;
    constexpr auto refl_rv_ref      = ^^lambda_rv_ref;
    constexpr auto refl_lv_ref_type = ^^decltype(lambda_lv_ref);
    constexpr auto refl_rv_ref_type = std::meta::type_of(refl_rv_ref);
    constexpr auto refl_obj = std::meta::reflect_constant(ret_lv_ref(a_lambda));

    static_assert(
        std::is_same_v<typename[:refl_lv_ref_type:], decltype(a_lambda) &>
    );
    static_assert(
        std::is_same_v<typename[:refl_rv_ref_type:], decltype(a_lambda) &&>
    );

    static_assert(ClosureVariable<refl_var>);
    static_assert(ClosureObject<refl_obj>);
    static_assert(!ClosureVariable<refl_lv_ref>);
    static_assert(!ClosureVariable<refl_rv_ref>);

    static_assert(!ClosureReferenceType<refl_lv_ref>);
    static_assert(!ClosureReferenceType<refl_rv_ref>);
    static_assert(ClosureReferenceType<refl_lv_ref_type>);
    static_assert(ClosureReferenceType<refl_rv_ref_type>);
    static_assert(ClosureReference<refl_lv_ref>);
    static_assert(ClosureReference<refl_rv_ref>);
    static_assert(!ClosureReference<refl_lv_ref_type>);
    static_assert(!ClosureReference<refl_rv_ref_type>);

    static_assert(CanExtractClosureType<refl_var>);
    static_assert(CanExtractClosureType<refl_obj>);
    static_assert(CanExtractClosureType<refl_type>);
    static_assert(CanExtractClosureType<refl_lv_ref>);
    static_assert(CanExtractClosureType<refl_lv_ref_type>);
    static_assert(CanExtractClosureType<refl_rv_ref>);
    static_assert(CanExtractClosureType<refl_rv_ref_type>);
    static_assert(!CanExtractClosureType<refl_class>);

    static_assert(std::meta::is_same_type(
        refl_type, extract_closure_type_refl<refl_var>()
    ));
    static_assert(std::meta::is_same_type(
        refl_type, extract_closure_type_refl<refl_obj>()
    ));
    static_assert(std::meta::is_same_type(
        refl_type, extract_closure_type_refl<refl_type>()
    ));
    static_assert(std::meta::is_same_type(
        refl_type, extract_closure_type_refl<refl_lv_ref>()
    ));
    static_assert(std::meta::is_same_type(
        refl_type, extract_closure_type_refl<refl_lv_ref_type>()
    ));
    static_assert(std::meta::is_same_type(
        refl_type, extract_closure_type_refl<refl_rv_ref>()
    ));
    static_assert(std::meta::is_same_type(
        refl_type, extract_closure_type_refl<refl_rv_ref_type>()
    ));

    static_assert(
        extract_closure_invoke_operator_refl<refl_var>() != std::meta::info { }
    );
    static_assert(
        extract_closure_invoke_operator_refl<refl_type>() != std::meta::info { }
    );
    static_assert(
        extract_closure_invoke_operator_refl<refl_lv_ref>() !=
        std::meta::info { }
    );
    static_assert(
        extract_closure_invoke_operator_refl<refl_lv_ref_type>() !=
        std::meta::info { }
    );
    static_assert(
        extract_closure_invoke_operator_refl<refl_rv_ref>() !=
        std::meta::info { }
    );
    static_assert(
        extract_closure_invoke_operator_refl<refl_rv_ref_type>() !=
        std::meta::info { }
    );
}
} // namespace static_tests
} // namespace usagi::meta::reflection
