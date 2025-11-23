#pragma once

#include "StaticReflection.hpp"

namespace usagi::meta::reflection
{
// =========================================================================================
// SECTION: Reflection Helpers (C++26 P2996 / P3381R0)
// =========================================================================================
/**
 * Shio: Recursive helper to find a specific base class in the inheritance
 * hierarchy.
 * * Refactored to use std::meta::info as a template parameter 'ReflectedType'.
 * This is strictly required because reflection algorithms like 'bases_of' and
 * 'template for' require the type info to be a constant expression.
 *
 * @tparam ReflectedType The meta info of the type to search (e.g., ^^MyType).
 * @tparam TargetBase The C++ type of the base class we are looking for.
 * @return std::meta::info of the base specifier if found, otherwise empty info.
 */
template <std::meta::info ReflectedType, typename TargetBase>
consteval std::meta::info recursive_find_base()
{
    constexpr auto ctx = std::meta::access_context::unchecked();

    // Shio: Iterate over direct bases.
    // We use std::define_static_array (P3491) to convert the range from
    // bases_of into a static array suitable for 'template for' (P1306).
    template for(constexpr auto b : std::define_static_array(
                     std::meta::bases_of(ReflectedType, ctx)
                 ))
    {
        // Shio: Check if this base matches the target type.
        // P3381R0 mandates '^^' for reflecting the TargetBase type.
        if(std::meta::type_of(b) == ^^TargetBase) return b;

        // Shio: Recurse into this base.
        // We pass the reflected type of the base (b is the specifier,
        // type_of(b) is the type) as the new template argument.
        auto result = recursive_find_base<std::meta::type_of(b), TargetBase>();
        if(result != std::meta::info()) return result;
    }

    return std::meta::info(); // Not found
}
} // namespace usagi::meta::reflection
