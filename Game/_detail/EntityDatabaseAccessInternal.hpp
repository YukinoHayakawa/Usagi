#pragma once

namespace usagi
{
template <
    typename Database
>
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
        return mDatabase->mEntityPages.begin();
    }

    auto entityPageEnd() const
    {
        return mDatabase->mEntityPages.end();
    }
};
}
