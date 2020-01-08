#pragma once

#include "ComponentFilter.hpp"

namespace usagi
{
template <
    typename Database,
    typename PermissionValidator,
    typename IncludeFilter,
    typename ExcludeFilter = ComponentFilter<>
>
class EntityDatabaseViewFiltered
{
public:
    using DatabaseT = Database;
    using PermissionCheckerT = PermissionValidator;

private:
    DatabaseT *mDatabase = nullptr;

public:
    explicit EntityDatabaseViewFiltered(Database *database)
        : mDatabase(database)
    {
    }

    auto begin()
    {

    }

    auto end()
    {

    }
};
}
