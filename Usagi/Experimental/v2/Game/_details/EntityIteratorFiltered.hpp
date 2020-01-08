#pragma once

#include "ComponentFilter.hpp"
#include "EntityIterator.hpp"

namespace usagi
{
template <
    typename Database,
    typename PermissionValidator,
    typename IncludeFilter,
    typename ExcludeFilter = ComponentFilter<>
>
class EntityIteratorFiltered
    : EntityIterator<Database, PermissionValidator>
{
    bool filterPredicate() const
    {
        auto view = this->currentView();
        const auto predicate =
            view.checkIncludeFilter(IncludeFilter())
            && view.checkExcludeFilter(ExcludeFilter());
        return predicate;
    }

    void satisfyFilter()
    {
        // todo check page mask first
        while(this->mPageCursor != this->entityPageEnd()
            && !filterPredicate())
        {
            BaseIteratorT::operator++();
        }
    }

public:
    using BaseIteratorT = EntityIterator<Database, PermissionValidator>;
    using BaseIteratorT::DatabaseT;
    using BaseIteratorT::PermissionValidatorT;
    using BaseIteratorT::PageIteratorT;

    // Standard Iterator Traits

    using BaseIteratorT::iterator_category;
    using BaseIteratorT::value_type;
    using BaseIteratorT::difference_type;
    using BaseIteratorT::pointer;
    using BaseIteratorT::reference;

    explicit EntityIteratorFiltered(
        DatabaseT *database)
        : BaseIteratorT(database)
    {
        satisfyFilter();
    }

    EntityIteratorFiltered(
        DatabaseT *database,
        PageIteratorT page_cursor,
        std::size_t index_in_page)
        : BaseIteratorT(database, std::move(page_cursor), index_in_page)
    {
        satisfyFilter();
    }

    EntityIteratorFiltered & operator++()
    {
        BaseIteratorT::operator++();
        satisfyFilter();
        return *this;
    }

    EntityIteratorFiltered operator++(int)
    {
        EntityIteratorFiltered ret = *this;
        ++(*this);
        return ret;
    }

    reference operator*() const
    {
        return BaseIteratorT::operator*();
    }

    // Equality Comparators

    friend bool operator==(
        const EntityIteratorFiltered &lhs, const EntityIteratorFiltered &rhs)
    {
        return static_cast<const BaseIteratorT &>(lhs) ==
            static_cast<const BaseIteratorT &>(rhs);
    }

    friend bool operator!=(
        const EntityIteratorFiltered &lhs, const EntityIteratorFiltered &rhs)
    {
        return !(lhs == rhs);
    }
};
}
