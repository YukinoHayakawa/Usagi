#pragma once

#include "PermissionValidatorReadOnly.hpp"
#include "EntityIterator.hpp"
#include "EntityDatabaseInternalAccess.hpp"

namespace usagi
{
/**
 * \brief Unfiltered Entity Database View.
 * \tparam Database
 */
template <
    typename Database
>
class EntityDatabaseViewUnfiltered
    : public EntityDatabaseInternalAccess<Database>
{
public:
    using DatabaseT = Database;
    using IteratorT = EntityIterator<
        DatabaseT,
        PermissionValidatorReadOnly
    >;

    explicit EntityDatabaseViewUnfiltered(Database *database)
        : EntityDatabaseInternalAccess<Database>(database)
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
