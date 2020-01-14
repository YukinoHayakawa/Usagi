
#include <Usagi/Experimental/v2/Game/_detail/ComponentFilter.hpp>
#include <Usagi/Experimental/v2/Game/_detail/ComponentAccessSystemAttribute.hpp>

namespace usagi
{
namespace
{
// ============================================================================

struct ComponentA
{
};

struct ComponentB
{
};

// ============================================================================

struct SystemAccessTraitTest
{
    using WriteAccess = ComponentFilter<ComponentA>;
};
static_assert(detail::ComponentWriteMaskBitPresent<
    SystemAccessTraitTest, ComponentA>::value
);

// TEST: Write access implies read access
static_assert(detail::HasComponentReadAccess<
    SystemAccessTraitTest, ComponentA
>);

// TEST: Explicit write access mask
static_assert(detail::HasComponentWriteAccess<
    SystemAccessTraitTest, ComponentA
>);

// TEST: No read access
static_assert(!detail::HasComponentReadAccess<
    SystemAccessTraitTest, ComponentB
>);

// TEST: No write access
static_assert(!detail::HasComponentWriteAccess<
    SystemAccessTraitTest, ComponentB
>);

// ============================================================================

struct SystemAccessTraitTest2
{
    using WriteAccess = ComponentFilter<ComponentA>;
    using ReadAccess = ComponentFilter<ComponentA, ComponentB>;
};

// TEST: Write access implies read access
static_assert(detail::HasComponentReadAccess<
    SystemAccessTraitTest2, ComponentA
>);

// TEST: Explicit read access mask
static_assert(detail::HasComponentReadAccess<
    SystemAccessTraitTest2, ComponentB
>);

// ============================================================================

struct SystemAccessTraitTest3
{
    using ReadAccess = ComponentFilter<ComponentA>;
};

// TEST: Read access does not imply write access
static_assert(!detail::HasComponentWriteAccess<
    SystemAccessTraitTest3, ComponentA
>);

// ============================================================================
}
}
