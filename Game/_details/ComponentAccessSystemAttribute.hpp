#pragma once

#include <Usagi/Experimental/v2/Game/Entity/Component.hpp>
#include <Usagi/Experimental/v2/Game/System.hpp>

namespace usagi
{
namespace details
{
template <System GameSystem, Component C>
struct ComponentReadAccess : std::integral_constant<
    bool,
    GameSystem::ReadAccess::template HAS_COMPONENT<C>
>
{
};

template <System GameSystem, Component C>
struct ComponentWriteAccess : std::integral_constant<
    bool,
    GameSystem::WriteAccess::template HAS_COMPONENT<C>
>
{
};

/*
struct ComponentReadAccessDefaultAllow;
struct ComponentWriteAccessDefaultAllow;

template <Component C>
struct ComponentReadAccess<ComponentReadAccessDefaultAllow, C>
    : std::integral_constant<bool, true>
{
};
template <Component C>
struct ComponentWriteAccess<ComponentWriteAccessDefaultAllow, C>
    : std::integral_constant<bool, true>
{
};
*/

template <System GameSystem, Component C>
constexpr bool ComponentWriteAccess_v =
    ComponentWriteAccess<GameSystem, C>::value;

template <System GameSystem, Component C>
constexpr bool ComponentReadAccess_v =
    ComponentWriteAccess_v<GameSystem, C> ||
    ComponentReadAccess<GameSystem, C>::value;

template <System GameSystem, Component C>
using ComponentReferenceType = std::conditional_t<
    ComponentWriteAccess_v<GameSystem, C>,
    C &,
    std::conditional_t<
        ComponentReadAccess_v<GameSystem, C>,
        const C &,
        void
    >
>;
}

template <System GameSystem>
struct ComponentAccessSystemAttribute
{
    template <Component C>
    static constexpr bool READ =
        details::ComponentReadAccess_v<GameSystem, C>;

    template <Component C>
    static constexpr bool WRITE =
        details::ComponentWriteAccess_v<GameSystem, C>;
};
}
