#pragma once

#include <Usagi/Entity/Archetype.hpp>

#include "EntityDatabaseViewFiltered.hpp"
#include "EntityDatabaseViewUnfiltered.hpp"
#include "EntityPageViewFiltered.hpp"

namespace usagi
{
template <
    typename Database,
    typename ComponentAccess
>
class EntityDatabaseAccessExternal
    : protected EntityDatabaseAccessInternal<Database>
{
public:
    using DatabaseT         = Database;
    using EntityPageT       = typename DatabaseT::EntityPageT;
    using ComponentAccessT  = ComponentAccess;

    EntityDatabaseAccessExternal() = default;

    explicit EntityDatabaseAccessExternal(Database *database)
        : EntityDatabaseAccessInternal<Database>(database)
    {
    }

    auto begin()
    {
        return this->entity_page_begin();
    }

    auto end()
    {
        return this->entity_page_end();
    }

    auto unfiltered_view()
    {
        return EntityDatabaseViewUnfiltered<
            DatabaseT,
            ComponentAccessT
        >(this->mDatabase);
    }

    template <Component... Include, Component... Exclude>
    auto view(
        ComponentFilter<Include...> include,
        // defaults to empty exclude mask via class template argument deduction
        ComponentFilter<Exclude...> exclude = { })
    {
        return EntityDatabaseViewFiltered<
            DatabaseT,
            ComponentAccessT,
            decltype(include),
            decltype(exclude)
        >(this->mDatabase);
    }

    // This can be used with begin(), end() to implement parallelization
    // with std::for_each. However, doing so might be inefficient due to the
    // amount of iterator pairs created. A better parallelization scheme
    // would divide the range of pages and directly iterate entities in the
    // divided ranges.
    template <Component... Include, Component... Exclude>
    auto page_view(
        const std::size_t page_idx,
        ComponentFilter<Include...> include,
        // defaults to empty exclude mask via class template argument deduction
        ComponentFilter<Exclude...> exclude = { })
    {
        return EntityPageViewFiltered<
            DatabaseT,
            ComponentAccessT,
            decltype(include),
            decltype(exclude)
        >(this->mDatabase, page_idx);
    }

    template <Component... InitialComponents>
    decltype(auto) create(Archetype<InitialComponents...> &archetype)
        requires (... && CanWriteComponent<ComponentAccessT, InitialComponents>)
    {
        return this->mDatabase->create(archetype);
    }
};
}
