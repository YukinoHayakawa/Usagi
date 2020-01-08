#pragma once

#include "PermissionValidatorReadOnly.hpp"
#include "EntityIterator.hpp"
#include "EntityDatabaseView.hpp"

namespace usagi
{
/**
 * \brief Unfiltered Entity Database View.
 * \tparam Database
 */
template <
    typename Database
>
class EntityDatabaseViewUnfiltered :
    public EntityDatabaseView<Database>
{
public:
    using typename EntityDatabaseView<Database>::DatabaseT;

    explicit EntityDatabaseViewUnfiltered(Database *database)
        : EntityDatabaseView<Database>(database)
    {
    }

    auto begin()
    {
        return EntityIterator<
            DatabaseT,
            PermissionValidatorReadOnly
        >(this->mDatabase, this->entityPageBegin(), 0);
    }

    auto end()
    {
        return EntityIterator<
            DatabaseT,
            PermissionValidatorReadOnly
        >(this->mDatabase, this->entityPageEnd(), 0);
    }
};
}
