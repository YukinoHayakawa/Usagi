#pragma once

#include <Usagi/Core/Clock.hpp>

#include "Entity.hpp"
#include "Execution.hpp"

namespace usagi::ecs
{
struct GameContext
{
    Clock master_clock;
    EntityManager entity_manager;
    Execution execution;
};
}
