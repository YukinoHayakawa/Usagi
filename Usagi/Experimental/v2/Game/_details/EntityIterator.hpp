#pragma once

#include <iterator>

#include "EntityView.hpp"

namespace usagi::ecs
{
template <
    typename Database,
    typename PermissionChecker
>
class EntityIterator
{
public:
    using DatabaseT = Database;
    using PermissionCheckerT = PermissionChecker;
    using PageIteratorT = decltype(DatabaseT::mEntityPages::allocatedBlocks());

    using iterator_category = std::forward_iterator_tag;
    using value_type = EntityView<DatabaseT, PermissionCheckerT>;
    using difference_type = void;
    using pointer = void;
    using reference = void;

    using DatabaseT::ENTITY_PAGE_SIZE;

private:
    DatabaseT &mDatabase;
    PageIteratorT mPageCursor = mDatabase.mEntityPages.begin();
    std::size_t mIndexInPage = 0;

public:
    explicit EntityIterator(Database &database)
        : mDatabase(database)
    {
    }

    EntityIterator & operator++()
    {
        ++mIndexInPage;
        if(mIndexInPage == DatabaseT::ENTITY_PAGE_SIZE)
        {
            ++mPageCursor;
            mIndexInPage = 0;
        }
        return *this;
    }

    EntityIterator operator++(int)
    {
        EntityIterator ret = *this;
        ++(*this);
        return ret;
    }

    reference operator*() const
    {
        EntityId id;

        id.id = mPageCursor->first_entity_id;
        id.id += mIndexInPage;
        id.reserved = 0; // not used yet

        return EntityView<DatabaseT, PermissionCheckerT>(mDatabase, id);
    }

    // Equality Comparators

    friend bool operator==(
        const EntityIterator &lhs, const EntityIterator &rhs)
    {
        return &lhs.mDatabase == &rhs.mDatabase
            && lhs.mPageCursor == rhs.mPageCursor
            && lhs.mIndexInPage == rhs.mIndexInPage;
    }

    friend bool operator!=(
        const EntityIterator &lhs, const EntityIterator &rhs)
    {
        return !(lhs == rhs);
    }
};
}
