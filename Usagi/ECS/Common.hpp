#pragma once

#include <cstdint>

namespace usagi::ecs
{
using EntityId = std::uint64_t;
constexpr inline EntityId INVALID_ENTITY_ID = -1;
constexpr inline std::size_t MAX_COMPONENTS = 64;
}
