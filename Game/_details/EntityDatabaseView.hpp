#pragma once

namespace usagi
{
template <
    typename Database
>
class EntityDatabaseView
{
public:
    using DatabaseT = Database;

protected:
    DatabaseT *mDatabase = nullptr;

    explicit EntityDatabaseView(Database *database)
        : mDatabase(database)
    {
    }

    auto entityPageBegin()
    {
        return mDatabase->mEntityPages.begin();
    }

    auto entityPageEnd()
    {
        return mDatabase->mEntityPages.end();
    }
};
}
