#pragma once

#include <Usagi/Entity/Component.hpp>

namespace usagi
{
template <typename C>
concept IsFlagComponent = Component<C> && requires
{
    typename C::FlagComponent;
};
}

#define USAGI_DECL_FLAG_COMPONENT(name) \
    struct name { using FlagComponent = void; } \
/**/
