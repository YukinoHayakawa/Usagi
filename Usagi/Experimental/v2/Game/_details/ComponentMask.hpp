#pragma once

// a bitset implementation supporting constexpr operations
#include <bitset2/bitset2.hpp>

#include <Usagi/Experimental/v2/Game/Entity/Component.hpp>

namespace usagi
{
template <Component... Components>
using ComponentMask = Bitset2::bitset2<sizeof...(Components)>;
}
