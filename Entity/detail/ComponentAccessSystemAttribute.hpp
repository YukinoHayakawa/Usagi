#pragma once

#include <Usagi/Entity/Component.hpp>
#include <Usagi/Entity/System.hpp>

#include "ComponentAccess.hpp"

namespace usagi
{
namespace detail
{
// ============================================================================
// ReadMaskBit
// ============================================================================

template <System GameSystem, Component C>
struct ComponentReadMaskBitPresent : std::false_type {};

template <System GameSystem, Component C>
    requires SystemDeclaresReadAccess<GameSystem>
struct ComponentReadMaskBitPresent<GameSystem, C>
    : std::bool_constant<GameSystem::ReadAccess::template HAS_COMPONENT<C>> {};

// ============================================================================
// WriteMaskBit
// ============================================================================

template <System GameSystem, Component C>
struct ComponentWriteMaskBitPresent : std::false_type {};

template <System GameSystem, Component C>
    requires SystemDeclaresWriteAccess<GameSystem>
struct ComponentWriteMaskBitPresent<GameSystem, C>
    : std::bool_constant<GameSystem::WriteAccess::template HAS_COMPONENT<C>> {};
}

// ============================================================================
// Component Access Implementation
// ============================================================================

template <System GameSystem>
struct ComponentAccessSystemAttribute
{
    template <Component C>
    static constexpr bool READ =
        detail::ComponentReadMaskBitPresent<GameSystem, C>::value;

    template <Component C>
    static constexpr bool WRITE =
        detail::ComponentWriteMaskBitPresent<GameSystem, C>::value;
};

// ============================================================================
// Derived Trait Flags
// ============================================================================

template <System GameSystem, Component C>
constexpr bool SystemCanWriteComponent =
    CanWriteComponent<ComponentAccessSystemAttribute<GameSystem>, C>;

template <System GameSystem, Component C>
constexpr bool SystemCanReadComponent =
    CanReadComponent<ComponentAccessSystemAttribute<GameSystem>, C>;

template <System GameSystem, Component C>
constexpr bool SystemCanAccessComponent =
    SystemCanWriteComponent<GameSystem, C>
    || SystemCanReadComponent<GameSystem, C>;

template <System GameSystem, Component C>
using ComponentReferenceType = std::conditional_t<
    SystemCanWriteComponent<GameSystem, C>,
    C &,
    std::conditional_t<
        SystemCanReadComponent<GameSystem, C>,
        const C &,
        void
    >
>;

template <System GameSystem, Component C>
constexpr ComponentAccess SystemHighestComponentAccess =
    SystemCanWriteComponent<GameSystem, C>
    ? ComponentAccess::WRITE
        : SystemCanReadComponent<GameSystem, C>
            ? ComponentAccess::READ
            : ComponentAccess::NONE;

// ============================================================================
// Explicit System Component Access Filters
// ============================================================================

template <
    System GameSystem,
    bool = SystemDeclaresReadAccess<GameSystem> &&
        !SystemDeclaresReadAllAccess<GameSystem>
>
struct ExplicitSystemComponentAccessTraitRead
{
    using type = ComponentFilter<>;
};

template <System GameSystem>
struct ExplicitSystemComponentAccessTraitRead<GameSystem, true>
{
    using type = typename GameSystem::ReadAccess;

    static_assert(IsComponentFilter<type>,
        "ReadAccess is not a component filter.");
};

template <System GameSystem>
using ExplicitSystemComponentAccessRead =
    typename ExplicitSystemComponentAccessTraitRead<GameSystem>::type;

template <
    System GameSystem,
    bool = SystemDeclaresWriteAccess<GameSystem> &&
        !SystemDeclaresWriteAllAccess<GameSystem>
>
struct ExplicitSystemComponentAccessTraitWrite
{
    using type = ComponentFilter<>;
};

template <System GameSystem>
struct ExplicitSystemComponentAccessTraitWrite<GameSystem, true>
{
    using type = typename GameSystem::WriteAccess;

    static_assert(IsComponentFilter<type>,
        "WriteAccess is not a component filter.");
};

template <System GameSystem>
using ExplicitSystemComponentAccessWrite =
    typename ExplicitSystemComponentAccessTraitWrite<GameSystem>::type;

template <typename GameSystem, typename Comp>
concept SystemExplicitComponentAccess =
    System<GameSystem>
    && Component<Comp>
    && (ExplicitSystemComponentAccessRead<GameSystem>::
        template HAS_COMPONENT<Comp> ||
        ExplicitSystemComponentAccessWrite<GameSystem>::
        template HAS_COMPONENT<Comp>)
;

template <System GameSystem>
using SystemExplicitComponentFilter =
    FilterDeduplicatedT<FilterConcatenatedT<
        ExplicitSystemComponentAccessRead<GameSystem>,
        ExplicitSystemComponentAccessWrite<GameSystem>
    >>;

// ============================================================================
// System Component Usage
// ============================================================================

template <System... Ss>
struct SystemComponentUsageHelper;

template <>
struct SystemComponentUsageHelper<>
{
    using type = ComponentFilter<>;
};

template <System S, System... Ss>
struct SystemComponentUsageHelper<S, Ss...>
{
    using type = FilterConcatenatedT<
        FilterConcatenatedT<
            ExplicitSystemComponentAccessRead<S>,
            ExplicitSystemComponentAccessWrite<S>
        >,
        typename SystemComponentUsageHelper<Ss...>::type
    >;
};

template <System... Ss>
using SystemComponentUsage = FilterDeduplicatedT<
    typename SystemComponentUsageHelper<Ss...>::type
>;
}
