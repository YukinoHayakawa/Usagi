#pragma once

#include <Usagi/Engine/Core/Element.hpp>

#include "Subsystem.hpp"

namespace usagi
{
template <typename... RequiredComponents>
class ConstrainedSubsystem : public Subsystem
{
public:
    bool handles(Element *entity) override
    {
        return (... && entity->hasComponent<RequiredComponents>());
    }
};
}
