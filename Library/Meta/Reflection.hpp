#pragma once

#include <meta>

// Header to be included if you want to use C++26 static reflection.

/*
 * This file also contains commonly used concepts that classify entities into
 * useful categories. For actual manipulations on the reflection objects, refer
 * to dedicated headers.
 */

namespace usagi::meta
{
// ********************************************************************* //
//                                Types                                  //
// ********************************************************************* //

/*
 * A type or a type alias that can be readily used as a concrete type or
 * dealiased.
 */
template <std::meta::info Refl>
concept TypeOrTypeAlias =
    std::meta::is_type(Refl) || std::meta::is_type_alias(Refl);

// Compatibility layer for P3687
consteval std::meta::info to_underlying(std::meta::info refl)
{
#if __has_feature(entity_proxy_reflection)
    return std::meta::underlying_entity_of(refl);
#else
    return std::meta::dealias(refl);
#endif
}

// Compatibility function for using declarations etc.
template <std::meta::info Refl>
consteval std::meta::info to_underlying()
{
    return to_underlying(Refl);
}

// ********************************************************************* //
//                               Templates                               //
// ********************************************************************* //

template <std::meta::info Refl>
concept TemplateSpecialization = std::meta::has_template_arguments(Refl);

// ********************************************************************* //
//                               Functions                               //
// ********************************************************************* //

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
} // namespace usagi::meta
