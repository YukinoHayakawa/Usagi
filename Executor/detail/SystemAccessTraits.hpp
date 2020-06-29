#pragma once

#include <array>

#include <Usagi/Entity/detail/ComponentAccess.hpp>

namespace usagi
{
template <std::size_t Size>
using SystemAccessTraits = std::array<ComponentAccess, Size>;
}
