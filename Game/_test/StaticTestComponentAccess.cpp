
#include <Usagi/Game/_detail/ComponentFilter.hpp>
#include <Usagi/Game/_detail/ComponentAccessSystemAttribute.hpp>

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
static_assert(SystemHasComponentReadAccess<
    SystemAccessTraitTest, ComponentA
>);

// TEST: Explicit write access mask
static_assert(SystemHasComponentWriteAccess<
    SystemAccessTraitTest, ComponentA
>);

// TEST: Mutable reference
static_assert(std::is_same_v<ComponentReferenceType<
    SystemAccessTraitTest, ComponentA
>, ComponentA &>);

// TEST: No read access
static_assert(!SystemHasComponentReadAccess<
    SystemAccessTraitTest, ComponentB
>);

// TEST: No reference (void)
static_assert(std::is_same_v<ComponentReferenceType<
    SystemAccessTraitTest, ComponentB
>, void>);

// TEST: No write access
static_assert(!SystemHasComponentWriteAccess<
    SystemAccessTraitTest, ComponentB
>);

// ============================================================================

struct SystemAccessTraitTest2
{
    using WriteAccess = ComponentFilter<ComponentA>;
    using ReadAccess = ComponentFilter<ComponentA, ComponentB>;
};

// TEST: Write access implies read access
static_assert(SystemHasComponentReadAccess<
    SystemAccessTraitTest2, ComponentA
>);

// TEST: Explicit read access mask
static_assert(SystemHasComponentReadAccess<
    SystemAccessTraitTest2, ComponentB
>);

// TEST: Const reference
static_assert(std::is_same_v<ComponentReferenceType<
    SystemAccessTraitTest2, ComponentB
>, const ComponentB &>);

// ============================================================================

struct SystemAccessTraitTest3
{
    using ReadAccess = ComponentFilter<ComponentA>;
};

// TEST: Read access does not imply write access
static_assert(!SystemHasComponentWriteAccess<
    SystemAccessTraitTest3, ComponentA
>);

// TEST: Const reference
static_assert(std::is_same_v<ComponentReferenceType<
    SystemAccessTraitTest3, ComponentA
>, const ComponentA &>);

// TEST: No reference
static_assert(std::is_same_v<ComponentReferenceType<
    SystemAccessTraitTest3, ComponentB
>, void>);

// ============================================================================
}
}
