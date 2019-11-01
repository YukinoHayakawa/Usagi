#pragma once

#include "Component.hpp"

namespace usagi::ecs
{
template <Component... InitialComponents>
class Archetype
{
    std::tuple<InitialComponents...> mInitialValues;

public:
    template <Component... EnabledComponents>
    Entity create(EntityDatabase<EnabledComponents...> &db)
    {

    }
};
}
