
#include <Usagi/Game/Database/EntityDatabase.hpp>
#include <Usagi/Game/Entity/Archetype.hpp>
#include <Usagi/Game/_detail/ComponentAccessSystemAttribute.hpp>
#include <Usagi/Game/_detail/EntityDatabaseAccessExternal.hpp>

namespace usagi
{
namespace
{
struct ComponentA
{
};

struct ComponentB
{
};

using Database = EntityDatabase<ComponentFilter<ComponentA, ComponentB>>;

using ArchetypeA = Archetype<ComponentA>;
using ArchetypeB = Archetype<ComponentB>;

struct System
{
    using WriteAccess = ArchetypeA::ComponentFilterT;
};

using Access = EntityDatabaseAccessExternal<
    Database,
    ComponentAccessSystemAttribute<System>
>;

template <typename T>
concept CanCreateA = requires (T)
{
    EntityDatabaseAccessExternal<
        Database,
        ComponentAccessSystemAttribute<T>
    >(nullptr).create(std::declval<ArchetypeA&>());
};

template <typename T>
concept CanCreateB = requires (T)
{
    EntityDatabaseAccessExternal<
        Database,
        ComponentAccessSystemAttribute<T>
    >(nullptr).create(std::declval<ArchetypeB&>());
};

static_assert(CanCreateA<System>);
static_assert(!CanCreateB<System>);
}
}
