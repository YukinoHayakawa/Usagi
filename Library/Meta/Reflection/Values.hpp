#pragma once

#include "StaticReflection.hpp"

namespace usagi::meta::reflection
{
template <std::meta::info Refl>
concept ValueOrObjectOrVariable = std::meta::is_object(Refl) ||
    std::meta::is_value(Refl) || std::meta::is_variable(Refl);
} // namespace usagi::meta::reflection
