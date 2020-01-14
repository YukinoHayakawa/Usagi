#pragma once

#include "ComponentFilter.hpp"
#include "EntityDatabaseViewUnfiltered.hpp"
#include "EntityIteratorFiltered.hpp"

namespace usagi
{
template <
    typename Database,
    typename ComponentAccess,
    typename IncludeFilter,
    typename ExcludeFilter = ComponentFilter<>
>
class EntityDatabaseViewFiltered
    : EntityDatabaseViewUnfiltered<Database, ComponentAccess>
{
    using BaseViewT = EntityDatabaseViewUnfiltered<
        Database, ComponentAccess
    >;

public:
    using DatabaseT = Database;
    using ComponentAccessT = ComponentAccess;
    using IncludeFilterT = IncludeFilter;
    using ExcludeFilterT = ExcludeFilter;
    using IteratorT = EntityIteratorFiltered<
        DatabaseT,
        ComponentAccessT,
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
