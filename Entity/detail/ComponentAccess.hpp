#pragma once

#include <Usagi/Entity/Component.hpp>

#include "ComponentFilter.hpp"

namespace usagi
{
// At places requiring access validations, use these two templates with
// ComponentAccess instead of using the ComponentAccess trait classes.

template <typename ComponentAccess, Component C>
constexpr bool CanWriteComponent =
    ComponentAccess::template WRITE<C>;

template <typename ComponentAccess, Component C>
constexpr bool CanReadComponent =
    CanWriteComponent<ComponentAccess, C> ||
    ComponentAccess::template READ<C>;

// Check write permissions for multiple components

template <typename ComponentAccess, Component... C>
constexpr bool CanWriteComponents =
    (... && CanWriteComponent<ComponentAccess, C>);

template <typename ComponentAccess, typename>
constexpr bool CanWriteComponentsByFilter = false;

template <typename ComponentAccess, Component... C>
constexpr bool CanWriteComponentsByFilter<
    ComponentAccess, ComponentFilter<C...>
> = CanWriteComponents<ComponentAccess, C...>;

// Check read permissions for multiple components

template <typename ComponentAccess, Component... C>
constexpr bool CanReadComponents =
(... && CanReadComponent<ComponentAccess, C>);

template <typename ComponentAccess, typename>
constexpr bool CanReadComponentsByFilter = false;

template <typename ComponentAccess, Component... C>
constexpr bool CanReadComponentsByFilter<
    ComponentAccess, ComponentFilter<C...>
> = CanReadComponents<ComponentAccess, C...>;

enum class ComponentAccess
{
    NONE = 0,
    READ = 1,
    WRITE = 2,
};
}
