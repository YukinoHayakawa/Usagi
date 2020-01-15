#pragma once

#include "EntityIterator.hpp"
#include "EntityDatabaseAccessInternal.hpp"

namespace usagi
{
/**
 * \brief Unfiltered Entity Database View.
 * \tparam Database
 */
template <
    typename Database,
    typename ComponentAccess
>
class EntityDatabaseViewUnfiltered
    : public EntityDatabaseAccessInternal<Database>
{
public:
    using DatabaseT = Database;
    using ComponentAccessT = ComponentAccess;
    using IteratorT = EntityIterator<
        DatabaseT,
        ComponentAccessT
    >;

    explicit EntityDatabaseViewUnfiltered(Database *database)
        : EntityDatabaseAccessInternal<Database>(database)
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
