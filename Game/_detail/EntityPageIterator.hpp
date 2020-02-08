#pragma once

#include <iterator>

namespace usagi
{
template <typename Database>
class EntityPageIterator
{
public:
    using iterator_category = std::input_iterator_tag;
    using value_type        = typename Database::EntityPageT;
    using difference_type   = int;
    using pointer           = value_type *;
    using reference         = value_type &;

private:
    Database *mDatabase = nullptr;
    std::size_t mPageIndex = value_type::INVALID_PAGE;

    reference dereference() const
    {
        return mDatabase->mEntityPages.at(mPageIndex);
    }

public:
    EntityPageIterator(Database *database, const std::size_t page_index)
        : mDatabase(database)
        , mPageIndex(page_index)
    {
    }

    EntityPageIterator & operator++()
    {
        assert(mPageIndex != value_type::INVALID_PAGE);

        mPageIndex = dereference().next_page;
        return *this;
    }

    EntityPageIterator operator++(int)
    {
        EntityPageIterator ret = *this;
        ++(*this);
        return ret;
    }

    reference operator*() const
    {
        return dereference();
    }

    pointer operator->() const
    {
        return &dereference();
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
