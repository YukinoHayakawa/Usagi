#pragma once

#include <Usagi/Game/Entity/Component.hpp>

namespace usagi
{
// At places requiring access validations, use these two templates with
// ComponentAccess instead of using the ComponentAccess trait classes.

template <typename ComponentAccess, Component C>
constexpr bool HasComponentWriteAccess =
    ComponentAccess::template WRITE<C>;

template <typename ComponentAccess, Component C>
constexpr bool HasComponentReadAccess =
    HasComponentWriteAccess<ComponentAccess, C> ||
    ComponentAccess::template READ<C>;

// Check write permissions for multiple components

template <typename ComponentAccess, Component... C>
constexpr bool HasComponentWriteAccesses =
    (... && HasComponentWriteAccess<ComponentAccess, C>);

// Note that ComponentFilter also qualifies as a Component so this
// specialization works.
template <typename ComponentAccess, Component... C>
constexpr bool HasComponentWriteAccesses<
    ComponentAccess, ComponentFilter<C...>
> = HasComponentWriteAccesses<ComponentAccess, C...>;

// Check read permissions for multiple components

template <typename ComponentAccess, typename... C>
constexpr bool HasComponentReadAccesses =
    (... && HasComponentReadAccess<ComponentAccess, C>);

template <typename ComponentAccess, Component... C>
constexpr bool HasComponentReadAccesses<
    ComponentAccess, ComponentFilter<C...>
> = HasComponentReadAccesses<ComponentAccess, C...>;
}
