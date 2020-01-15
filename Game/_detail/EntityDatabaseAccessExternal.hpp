#pragma once

#include "EntityDatabaseViewFiltered.hpp"
#include "EntityDatabaseViewUnfiltered.hpp"

namespace usagi
{
template <
    typename Database,
    typename ComponentAccess
>
class EntityDatabaseAccessExternal
{
public:
    using DatabaseT = Database;
    using ComponentAccessT = ComponentAccess;

private:
    DatabaseT *mDatabase = nullptr;

public:
    explicit EntityDatabaseAccessExternal(Database *database)
        : mDatabase(database)
    {
    }

    template <Component... Include, Component... Exclude>
    auto view(
        ComponentFilter<Include...> include,
        // defaults to empty exclude mask via class template argument deduction
        ComponentFilter<Exclude...> exclude = { })
    {
        return EntityDatabaseViewFiltered<
            DatabaseT,
            ComponentAccessT,
            decltype(include),
            decltype(exclude)
        >(mDatabase);
    }

    auto unfilteredView()
    {
        return EntityDatabaseViewUnfiltered<
            DatabaseT,
            ComponentAccessT
        >(mDatabase);
    }

    template <
        Component... InitialComponents,
        typename EntityIdOutputIterator = std::nullptr_t
    >
    decltype(auto) create(
        Archetype<InitialComponents...> &archetype,
        const std::size_t count = 1,
        EntityIdOutputIterator &&id_output = nullptr
    ) requires (... && HasComponentWriteAccess<
        ComponentAccessT, InitialComponents
    >)
    {
        return mDatabase->create(
            archetype, count,
            std::forward<EntityIdOutputIterator>(id_output)
        );
    }
};
}
