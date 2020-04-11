#pragma once

#include <Usagi/Game/Entity/Component.hpp>
#include <Usagi/Game/System.hpp>

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
    requires SystemHasReadAccessMask<GameSystem>
struct ComponentReadMaskBitPresent<GameSystem, C>
    : std::bool_constant<GameSystem::ReadAccess::template HAS_COMPONENT<C>> {};

// ============================================================================
// WriteMaskBit
// ============================================================================

template <System GameSystem, Component C>
struct ComponentWriteMaskBitPresent : std::false_type {};

template <System GameSystem, Component C>
    requires SystemHasWriteAccessMask<GameSystem>
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
        detail::ComponentReadMaskBitPresent<GameSystem, C>::value
        || SystemHasReadAllAccess<GameSystem>;

    template <Component C>
    static constexpr bool WRITE =
        detail::ComponentWriteMaskBitPresent<GameSystem, C>::value
        || SystemHasWriteAllAccess<GameSystem>;
};

// ============================================================================
// Derived Trait Flags
// ============================================================================

template <System GameSystem, Component C>
constexpr bool SystemHasComponentWriteAccess =
    HasComponentWriteAccess<ComponentAccessSystemAttribute<GameSystem>, C>;

template <System GameSystem, Component C>
constexpr bool SystemHasComponentReadAccess =
    HasComponentReadAccess<ComponentAccessSystemAttribute<GameSystem>, C>;

template <System GameSystem, Component C>
using ComponentReferenceType = std::conditional_t<
    SystemHasComponentWriteAccess<GameSystem, C>,
    C &,
    std::conditional_t<
        SystemHasComponentReadAccess<GameSystem, C>,
        const C &,
        void
    >
>;
}
