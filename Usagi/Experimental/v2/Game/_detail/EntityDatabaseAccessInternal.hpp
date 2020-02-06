#pragma once

namespace usagi
{
template <typename Database>
class EntityDatabaseAccessInternal
{
public:
    using DatabaseT = Database;

protected:
    DatabaseT *mDatabase = nullptr;

    explicit EntityDatabaseAccessInternal(Database *database)
        : mDatabase(database)
    {
    }

    auto entityPageBegin() const
    {
        return EntityPageIterator<DatabaseT>(
            mDatabase, mDatabase->mFirstEntityPageIndex
        );
    }

    auto entityPageEnd() const
    {
        return EntityPageIterator<DatabaseT>(
            mDatabase, DatabaseT::EntityPageT::INVALID_PAGE
        );
    }
};
}
