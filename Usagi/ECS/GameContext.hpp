#pragma once

#include <Usagi/Core/Clock.hpp>

#include "Execution.hpp"

namespace usagi::ecs
{
struct GameContext
{
    Clock master_clock;
    EntityManager entity_manager;
    Execution execution { &entity_manager };
};
}
