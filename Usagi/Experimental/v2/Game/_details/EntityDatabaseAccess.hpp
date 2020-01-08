#pragma once

#include "EntityDatabaseViewFiltered.hpp"
#include "EntityDatabaseViewUnfiltered.hpp"

namespace usagi
{
template <
    typename Database,
    typename PermissionValidator
>
class EntityDatabaseAccess
{
public:
    using DatabaseT = Database;
    using PermissionValidatorT = PermissionValidator;

private:
    DatabaseT *mDatabase = nullptr;

public:
    explicit EntityDatabaseAccess(Database *database)
        : mDatabase(database)
    {
    }

    template <
        typename ComponentFilterInclude,
        typename ComponentFilterExclude
    >
    auto view()
    {
        return EntityDatabaseViewFiltered<
            DatabaseT,
            PermissionValidatorT,
            ComponentFilterInclude,
            ComponentFilterExclude
        >(mDatabase);
    }

    auto fullView()
    {
        return EntityDatabaseViewUnfiltered<DatabaseT>(mDatabase);
    }
};
}
