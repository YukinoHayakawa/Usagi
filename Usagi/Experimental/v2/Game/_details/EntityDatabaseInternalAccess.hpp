#pragma once

namespace usagi
{
template <
    typename Database
>
class EntityDatabaseInternalAccess
{
public:
    using DatabaseT = Database;

protected:
    DatabaseT *mDatabase = nullptr;

    explicit EntityDatabaseInternalAccess(Database *database)
        : mDatabase(database)
    {
    }

    auto entityPageBegin() const
    {
        return mDatabase->mEntityPages.begin();
    }

    auto entityPageEnd() const
    {
        return mDatabase->mEntityPages.end();
    }
};
}
