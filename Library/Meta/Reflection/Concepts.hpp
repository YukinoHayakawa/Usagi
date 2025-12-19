#pragma once

#include <Usagi/Library/Meta/Templates/SFINAE.hpp>

#include "StaticReflection.hpp"

// Commonly used concepts are put in this file.
namespace usagi::meta::reflection
{
template <std::meta::info Refl>
concept FunctionOrFunctionType =
    std::meta::is_function(Refl) || std::meta::is_function_type(Refl);

template <std::meta::info Refl>
concept OperatorFunctionOrTemplate =
    std::meta::is_operator_function_template(Refl) ||
    std::meta::is_operator_function(Refl);

template <std::meta::info Refl, std::meta::operators Op>
concept OperatorFunctionOrTemplateOf = requires {
    typename sfinae_probe_value<std::meta::operator_of(Refl)>;
} && std::meta::operator_of(Refl) == Op;
} // namespace usagi::meta::reflection
