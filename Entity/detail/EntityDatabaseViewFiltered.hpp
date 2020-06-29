#pragma once

#include "ComponentFilter.hpp"
#include "EntityDatabaseAccessInternal.hpp"
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
    : EntityDatabaseAccessInternal<Database>
{
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
        : EntityDatabaseAccessInternal<Database>(database)
    {
    }

    auto begin()
    {
        return IteratorT(
            this->mDatabase, this->entity_page_begin(), 0,
            { this->mDatabase, this->entity_page_end(), 0 }
        );
    }

    auto end()
    {
        return IteratorT(
            this->mDatabase, this->entity_page_end(), 0,
            { this->mDatabase, this->entity_page_end(), 0 }
        );
    }
};
}
