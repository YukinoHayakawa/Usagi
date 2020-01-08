﻿#pragma once

#include <iterator>

#include "EntityView.hpp"
#include "EntityDatabaseInternalAccess.hpp"

namespace usagi
{
/**
 * \brief
 *
 * Iterator invalidation:
 * -- When Entity Page is created or destroyed.
 *
 * \tparam Database
 * \tparam PermissionValidator
 */
template <
    typename Database,
    typename PermissionValidator
>
class EntityIterator
    : protected EntityDatabaseInternalAccess<Database>
{
public:
    using DatabaseT             = Database;
    using PermissionValidatorT  = PermissionValidator;
    using PageIteratorT         = typename DatabaseT::EntityPageIteratorT;

    // Standard Iterator Traits

    // todo: fix: in fact, this iterator returns a proxy object when dereferenced. since the proxy object is actually not a reference, this iterator does not satisfy any iterator category requirement.
    // boost::single_pass_traversal_tag
    using iterator_category = std::input_iterator_tag;
    using value_type        = EntityView<DatabaseT, PermissionValidatorT>;
    using difference_type   = void;
    using pointer           = void;
    using reference         = value_type;

protected:
    PageIteratorT   mPageCursor     = this->entityPageBegin();
    std::size_t     mIndexInPage    = 0;

    auto currentView() const
    {
        assert(this->mDatabase);
        assert(mPageCursor != this->entityPageEnd());
        assert(mIndexInPage < DatabaseT::ENTITY_PAGE_SIZE);

        return EntityView<DatabaseT, PermissionValidatorT>(
            this->mDatabase, &(*mPageCursor).data, mIndexInPage);
    }

public:
    explicit EntityIterator(DatabaseT *database)
        : EntityDatabaseInternalAccess<Database>(database)
    {
    }

    EntityIterator(
        DatabaseT *database,
        PageIteratorT page_cursor,
        const std::size_t index_in_page)
        : EntityDatabaseInternalAccess<Database>(database)
        , mPageCursor(std::move(page_cursor))
        , mIndexInPage(index_in_page)
    {
    }

    EntityIterator & operator++()
    {
        assert(mPageCursor != this->entityPageEnd());

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
        return currentView();
    }

    // Equality Comparators

    friend bool operator==(
        const EntityIterator &lhs, const EntityIterator &rhs)
    {
        assert(lhs.mDatabase == rhs.mDatabase);

        return lhs.mDatabase == rhs.mDatabase
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
