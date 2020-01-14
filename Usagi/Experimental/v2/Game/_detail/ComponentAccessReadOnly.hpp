#pragma once

#include <Usagi/Experimental/v2/Game/Entity/Component.hpp>

namespace usagi
{
struct ComponentAccessReadOnly
{
    template <Component C>
    static constexpr bool READ = true;

    template <Component C>
    static constexpr bool WRITE = false;
};
}
