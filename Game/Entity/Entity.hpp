#pragma once

#include "Component.hpp"

namespace usagi::ecs
{
/**
 * \brief A proxy class for manipulating entity components.
 * \tparam EnabledComponents
 */
template <Component... EnabledComponents>
class Entity
{
    using DatabaseT = EntityDatabase<EnabledComponents...>;

    DatabaseT &mDatabase;

};
}
