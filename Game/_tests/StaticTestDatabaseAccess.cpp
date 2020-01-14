
#include <Usagi/Experimental/v2/Game/Database/EntityDatabase.hpp>
#include <Usagi/Experimental/v2/Game/Entity/Archetype.hpp>
#include <Usagi/Experimental/v2/Game/_detail/ComponentAccessSystemAttribute.hpp>
#include <Usagi/Experimental/v2/Game/_detail/EntityDatabaseExternalAccess.hpp>

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

using Database = EntityDatabase<16, ComponentA, ComponentB>;

using ArchetypeA = Archetype<ComponentA>;
using ArchetypeB = Archetype<ComponentB>;

struct System
{
    using WriteAccess = ArchetypeA::ComponentFilterT;
};

using Access = EntityDatabaseExternalAccess<
    Database,
    ComponentAccessSystemAttribute<System>
>;

template <typename T>
concept CanCreateA = requires (T t)
{
    EntityDatabaseExternalAccess<
        Database,
        ComponentAccessSystemAttribute<T>
    >(nullptr).create(ArchetypeA());
};

template <typename T>
concept CanCreateB = requires (T t)
{
    EntityDatabaseExternalAccess<
        Database,
        ComponentAccessSystemAttribute<T>
    >(nullptr).create(ArchetypeB());
};

static_assert(CanCreateA<System>);
static_assert(!CanCreateB<System>);
}
}
