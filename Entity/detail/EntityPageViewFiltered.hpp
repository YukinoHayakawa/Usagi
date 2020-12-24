#pragma once

#include "ComponentFilter.hpp"
#include "EntityIteratorFiltered.hpp"

namespace usagi
{
template <
    typename Database,
    typename ComponentAccess,
    typename IncludeFilter,
    typename ExcludeFilter = ComponentFilter<>
>
class EntityPageViewFiltered
    : protected EntityDatabaseAccessInternal<Database>
{
    static_assert(CanReadComponentsByFilter<ComponentAccess, IncludeFilter>);
    static_assert(CanReadComponentsByFilter<ComponentAccess, ExcludeFilter>);

    std::size_t mPageIndex = EntityPageT::INVALID_PAGE;

public:
    using DatabaseT = Database;
    using EntityPageT = typename Database::EntityPageT;
    using ComponentAccessT = ComponentAccess;
    using IncludeFilterT = IncludeFilter;
    using ExcludeFilterT = ExcludeFilter;
    using IteratorT = EntityIteratorFiltered<
        DatabaseT,
        ComponentAccessT,
        IncludeFilterT,
        ExcludeFilterT
    >;

    EntityPageViewFiltered() = default;

    explicit EntityPageViewFiltered(DatabaseT *database, const std::size_t idx)
        : EntityDatabaseAccessInternal<Database>(database)
        , mPageIndex(idx)
    {
    }

    auto begin()
    {
        auto this_page = this->entity_page_at_index(mPageIndex);
        auto next_page = this_page;
        ++next_page;
        return IteratorT(
            this->mDatabase,
            this_page,
            0,
            { this->mDatabase, next_page, 0 }
        );
    }

    auto end()
    {
        const auto next_page = ++this->entity_page_at_index(mPageIndex);
        return IteratorT(
            this->mDatabase,
            next_page,
            0,
            { this->mDatabase, next_page, 0 }
        );
    }
};
}
