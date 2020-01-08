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
    : EntityDatabaseViewUnfiltered<Database>
{
    using BaseViewT = EntityDatabaseViewUnfiltered<Database>;

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
        : EntityDatabaseViewUnfiltered<Database>(database)
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
