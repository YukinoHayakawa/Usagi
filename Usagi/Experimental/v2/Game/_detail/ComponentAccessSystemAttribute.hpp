#pragma once

#include <Usagi/Experimental/v2/Game/Entity/Component.hpp>
#include <Usagi/Experimental/v2/Game/System.hpp>

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

// ============================================================================
// Derived Trait Flags
// ============================================================================

template <System GameSystem, Component C>
constexpr bool HasComponentWriteAccess =
    ComponentWriteMaskBitPresent<GameSystem, C>::value;

template <System GameSystem, Component C>
constexpr bool HasComponentReadAccess =
    HasComponentWriteAccess<GameSystem, C> ||
    ComponentReadMaskBitPresent<GameSystem, C>::value;

template <System GameSystem, Component C>
using ComponentReferenceType = std::conditional_t<
    HasComponentWriteAccess<GameSystem, C>,
    C &,
    std::conditional_t<
        HasComponentReadAccess<GameSystem, C>,
        const C &,
        void
    >
>;
}

// ============================================================================
// Component Access Implementation
// ============================================================================

template <System GameSystem>
struct ComponentAccessSystemAttribute
{
    template <Component C>
    static constexpr bool READ =
        detail::HasComponentReadAccess<GameSystem, C>;

    template <Component C>
    static constexpr bool WRITE =
        detail::HasComponentWriteAccess<GameSystem, C>;
};
}
