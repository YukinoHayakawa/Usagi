#pragma once

#include <Usagi/Game/Entity/Component.hpp>

namespace usagi
{
/**
 * \brief For external access to the Entities, such as Game Scripts.
 * Do not use this for Systems.
 */
struct ComponentAccessAllowAll
{
    template <Component C>
    static constexpr bool READ = true;

    template <Component C>
    static constexpr bool WRITE = true;
};
}
