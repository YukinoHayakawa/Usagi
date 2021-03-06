﻿#pragma once

#include <iterator>
#include <cassert>

namespace usagi
{
template <typename Database>
class EntityPageIterator
{
public:
    using iterator_category = std::forward_iterator_tag;
    using value_type        = typename Database::EntityPageT;
    using difference_type   = std::ptrdiff_t;
    using pointer           = value_type *;
    using reference         = std::size_t;

private:
    Database *mDatabase = nullptr;
    std::size_t mPageIndex = value_type::INVALID_PAGE;

public:
    EntityPageIterator() = default;

    EntityPageIterator(Database *database, const std::size_t page_index)
        : mDatabase(database)
        , mPageIndex(page_index)
    {
    }

    EntityPageIterator & operator++()
    {
        assert(mPageIndex != value_type::INVALID_PAGE);

        mPageIndex = ref().next_page;
        return *this;
    }

    EntityPageIterator operator++(int)
    {
        EntityPageIterator ret = *this;
        ++(*this);
        return ret;
    }

    auto index() const
    {
        return mPageIndex;
    }

    auto & ref() const
    {
        assert(mPageIndex != value_type::INVALID_PAGE);
        auto &p = mDatabase->entity_pages().at(mPageIndex);
        // check for deallocated pages
        assert(p.page_seq_id != -1);
        return p;
    }

    reference operator*() const
    {
        return index();
    }

    pointer operator->() const
    {
        return &ref();
    }

    // Equality Comparators

    friend bool operator==(
        const EntityPageIterator &lhs,
        const EntityPageIterator &rhs)
    {
        return lhs.mDatabase == rhs.mDatabase
            && lhs.mPageIndex == rhs.mPageIndex;
    }

    friend bool operator!=(
        const EntityPageIterator &lhs,
        const EntityPageIterator &rhs)
    {
        return !(lhs == rhs);
    }
};
}
