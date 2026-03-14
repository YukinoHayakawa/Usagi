#pragma once

#include <meta>


namespace usagi::meta::reflection
{


/*
// https://en.cppreference.com/w/cpp/language/type_alias.html
// An alias template refers to a family of types which could be class templates,
// function pointers, fundamental types, or whatever. It doesn't seem like
// P2996 is providing a method allowing us to know it.
// todo: implement metafunctions for getting what's behind the alias template.
template <std::meta::info Refl>
concept ClassTemplate/*OrAlias#1# =
    std::meta::is_class_template(Refl);
// todo: || (std::meta::is_alias_template(Refl) &&
//   std::meta::is_class_template(std::meta::

template <std::meta::info Refl>
concept TypeOrClassTemplate =
    TypeOrTypeAlias<Refl> || std::meta::is_class_template(Refl);

template <std::meta::info Refl>
concept Function = std::meta::is_function(Refl);

template <std::meta::info Refl>
concept FunctionOrFunctionType =
    std::meta::is_function(Refl) || std::meta::is_function_type(Refl);

template <std::meta::info Refl>
concept FreeFunction = Function<Refl> &&
    (!std::meta::is_class_member(Refl) || std::meta::is_static_member(Refl));

template <std::meta::info Refl>
concept FunctionTemplate = std::meta::is_function_template(Refl);

template <std::meta::info Refl>
concept OperatorFunctionOrTemplate =
    std::meta::is_operator_function_template(Refl) ||
    std::meta::is_operator_function(Refl);
    */

/*
 * todo: sfinae_probe_value is not correct C++.
template <std::meta::info Refl, std::meta::operators Op>
concept OperatorFunctionOrTemplateOf = requires {
    typename sfinae_probe_value<std::meta::operator_of(Refl)>;
} && std::meta::operator_of(Refl) == Op;
*/
} // namespace usagi::meta::reflection
