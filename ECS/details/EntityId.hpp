#pragma once

#include <cstdint>

namespace usagi::ecs
{
struct EntityId
{
    std::uint64_t reserved : 16;
    std::uint64_t id : 48;
};
}
