#pragma once

#include <bitset>

#include "../Component.hpp"

namespace usagi::ecs
{
template <Component... Components>
using ComponentMask = std::bitset<sizeof...(Components)>;
}
