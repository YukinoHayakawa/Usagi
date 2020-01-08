#pragma once

#include "ComponentFilter.hpp"
#include "EntityDatabaseViewUnfiltered.hpp"
#include "EntityIteratorFiltered.hpp"

namespace usagi
{
template <
    typename Database,
    typename PermissionValidator,
    typename IncludeFilter,
    typename ExcludeFilter = ComponentFilter<>
>
class EntityDatabaseViewFiltered
    : EntityDatabaseViewUnfiltered<Database, PermissionValidator>
{
    using BaseViewT = EntityDatabaseViewUnfiltered<
        Database, PermissionValidator
    >;

public:
    using DatabaseT = Database;
    using PermissionValidatorT = PermissionValidator;
    using IncludeFilterT = IncludeFilter;
    using ExcludeFilterT = ExcludeFilter;
    using IteratorT = EntityIteratorFiltered<
        DatabaseT,
        PermissionValidatorT,
        IncludeFilterT,
        ExcludeFilterT
    >;

    explicit EntityDatabaseViewFiltered(DatabaseT *database)
        : BaseViewT(database)
    {
    }

    auto begin()
    {
        return IteratorT(this->mDatabase, this->entityPageBegin(), 0);
    }

    auto end()
    {
        return IteratorT(this->mDatabase, this->entityPageEnd(), 0);
    }
};
}
