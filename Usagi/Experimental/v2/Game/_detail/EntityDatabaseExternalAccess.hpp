#pragma once

#include "EntityDatabaseViewFiltered.hpp"
#include "EntityDatabaseViewUnfiltered.hpp"

namespace usagi
{
template <
    typename Database,
    typename ComponentAccess
>
class EntityDatabaseExternalAccess
{
public:
    using DatabaseT = Database;
    using ComponentAccessT = ComponentAccess;

private:
    DatabaseT *mDatabase = nullptr;

public:
    explicit EntityDatabaseExternalAccess(Database *database)
        : mDatabase(database)
    {
    }

    template <
        Component... Include,
        Component... Exclude
    >
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

    template <Component... InitialComponents>
    decltype(auto) create(
        const Archetype<InitialComponents...> &archetype,
        const std::size_t count = 1
    ) requires (... && HasComponentWriteAccess<
        ComponentAccessT, InitialComponents
    >)
    {
        return mDatabase->create(archetype, count);
    }
};
}
