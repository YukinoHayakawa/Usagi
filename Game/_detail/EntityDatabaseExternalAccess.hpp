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
        ComponentFilter<Exclude...> exclude = { }) // default arguments via CTAD
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
};
}
