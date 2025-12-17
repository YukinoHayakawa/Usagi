#pragma once

#include "StaticReflection.hpp"

namespace usagi::meta::reflection
{
// A variation of `std::define_static_array`.
consteval auto make_static_subarray(auto && range, const std::size_t size)
{
    using elem_t          = std::ranges::range_value_t<decltype(range)>;
    const auto range_refl = std::meta::reflect_constant_array(range);
    return std::span<const elem_t>(extract<const elem_t *>(range_refl), size);
}
} // namespace usagi::meta::reflection
