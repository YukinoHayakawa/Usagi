#pragma once

#include <Usagi/Engine/Entity/Entity.hpp>

#include "Subsystem.hpp"

namespace usagi
{
template <typename... RequiredComponents>
class ConstrainedSubsystem : public Subsystem
{
public:
    bool handles(Entity *entity) override
    {
        return (... && entity->hasComponent<RequiredComponents>());
    }
};
}
