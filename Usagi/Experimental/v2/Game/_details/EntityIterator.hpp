#pragma once

#include <iterator>

#include "EntityView.hpp"

namespace usagi
{
template <
    typename Database,
    typename PermissionValidator
>
class EntityIterator
{
public:
    using DatabaseT             = Database;
    using PermissionValidatorT  = PermissionValidator;
    using PageIteratorT         = typename DatabaseT::EntityPageIteratorT;

    using DatabaseT::ENTITY_PAGE_SIZE;

    // Standard Iterator Traits

    using iterator_category = std::forward_iterator_tag;
    using value_type        = EntityView<DatabaseT, PermissionValidatorT>;
    using difference_type   = void;
    using pointer           = void;
    using reference         = void;

private:
    DatabaseT       *mDatabase;
    PageIteratorT   mPageCursor     = mDatabase->mEntityPages.begin();
    std::size_t     mIndexInPage    = 0;

public:
    explicit EntityIterator(DatabaseT *database)
        : mDatabase(database)
    {
    }

    EntityIterator(
        DatabaseT *database,
        PageIteratorT page_cursor,
        const std::size_t index_in_page)
        : mDatabase(database)
        , mPageCursor(std::move(page_cursor))
        , mIndexInPage(index_in_page)
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
        return EntityView<DatabaseT, PermissionValidatorT>(
            mDatabase, &*mPageCursor, mIndexInPage);
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
