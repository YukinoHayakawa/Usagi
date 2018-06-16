#pragma once

#include "Subsystem.hpp"
#include "Entity.hpp"

namespace yuki
{
template <typename... RequiredComponents>
class ConstrainedSubsystem : public Subsystem
{
public:
    bool canProcess(Entity *entity) override
    {
        return (... && entity->hasComponent<RequiredComponents>());
    }
};
}
