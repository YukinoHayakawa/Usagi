#pragma once

#include "EntityIterator.hpp"
#include "EntityDatabaseInternalAccess.hpp"

namespace usagi
{
/**
 * \brief Unfiltered Entity Database View.
 * \tparam Database
 */
template <
    typename Database,
    typename PermissionValidator
>
class EntityDatabaseViewUnfiltered
    : public EntityDatabaseInternalAccess<Database>
{
public:
    using DatabaseT = Database;
    using PermissionValidatorT = PermissionValidator;
    using IteratorT = EntityIterator<
        DatabaseT,
        PermissionValidatorT
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
