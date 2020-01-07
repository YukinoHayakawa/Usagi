#pragma once

#include <bitset>

#include <Usagi/Experimental/v2/Game/Entity/Component.hpp>

namespace usagi
{
template <Component... Components>
using ComponentMask = std::bitset<sizeof...(Components)>;
}
