#pragma once

// BMI1 (Bit Manipulation Instruction Set 1) intrinsics
#include <immintrin.h>

#include "ComponentFilter.hpp"
#include "EntityIterator.hpp"

namespace usagi
{
template <
    typename Database,
    typename ComponentAccess,
    typename IncludeFilter,
    typename ExcludeFilter = ComponentFilter<>
>
class EntityIteratorFiltered
    : EntityIterator<Database, ComponentAccess>
{
public:
    using BaseIteratorT = EntityIterator<Database, ComponentAccess>;
    using BaseIteratorT::DatabaseT;
    using BaseIteratorT::ComponentAccessT;
    using BaseIteratorT::PageIteratorT;
    using BaseIteratorT::EntityPageT;
    using BaseIteratorT::EntityIndexT;

    // Standard Iterator Traits

    using BaseIteratorT::iterator_category;
    using BaseIteratorT::value_type;
    using BaseIteratorT::difference_type;
    using BaseIteratorT::pointer;
    using BaseIteratorT::reference;

private:
    typename EntityPageT::EntityArrayT mEntityIncludeMask;
    typename EntityPageT::EntityArrayT mEntityExcludeMask;
    typename EntityPageT::EntityArrayT mEntityFiltered;

    bool mUnencounteredPage = true;

    // only pay for what you use: only check for those components you are
    // interested in + early exit. more precise than bit-or of all entities.
    // write more representative Component types at the front of the type list
    // for more efficient filtering.

    template <Component C>
    bool check_include_component()
    {
        const auto mask = this->mPageCursor->
            template component_enabled_mask<C>();

        // no entity in this page has the designated component
        if(mask == 0)
            return false;

        // Conditional conjunction
        mEntityIncludeMask &= mask;

        return true;
    }

    template <Component... C>
    bool check_include_components(ComponentFilter<C...>)
    {
        // guaranteed short circuiting based on left fold
        const bool predicate = (... && check_include_component<C>());
        return predicate;
    }

    template <Component C>
    bool check_exclude_component()
    {
        const auto mask = this->mPageCursor->
            template component_enabled_mask<C>();

        // all entity in this page have excluded component
        if(mask == -1)
            return false;

        // Conditional disjunction
        mEntityExcludeMask |= mask;

        return true;
    }

    template <Component... C>
    bool check_exclude_components(ComponentFilter<C...>)
    {
        // guaranteed short circuiting based on left fold
        const bool predicate = (... && check_exclude_component<C>());
        return predicate;
    }

    void increment()
    {
        while(this->mPageCursor != this->entityPageEnd())
        {
            // Encountered a new page, perform early check
            if(mUnencounteredPage)
            {
                mEntityIncludeMask = -1;
                mEntityExcludeMask = 0;

                // if mEntityIncludeMask == 0, it means even all types
                // of components exist in some entities, there is no
                // single entity has all of them.
                if(!check_include_components(IncludeFilter())
                    || mEntityIncludeMask == 0
                    || !check_exclude_components(ExcludeFilter()))
                {
                    this->next_page();
                    continue;
                }

                // The entities which meet the filters
                mEntityFiltered = mEntityIncludeMask & ~mEntityExcludeMask;

                mUnencounteredPage = false;
            }

            // No eligible entity left in this page.
            if(mEntityFiltered == 0)
            {
                this->next_page();
                mUnencounteredPage = true;
                continue;
            }

            // We found an eligible entity
            // bug: unportable code
            this->mIndexInPage = _tzcnt_u32(mEntityFiltered);
            mEntityFiltered = _blsr_u32(mEntityFiltered);
            break;
        }
    }

public:
    explicit EntityIteratorFiltered(
        DatabaseT *database)
        : BaseIteratorT(database)
    {
        increment();
    }

    EntityIteratorFiltered(
        DatabaseT *database,
        PageIteratorT page_cursor,
        const EntityIndexT index_in_page)
        : BaseIteratorT(database, std::move(page_cursor), index_in_page)
    {
        increment();
    }

    EntityIteratorFiltered & operator++()
    {
        // DO NOT call the base iterator
        increment();
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
