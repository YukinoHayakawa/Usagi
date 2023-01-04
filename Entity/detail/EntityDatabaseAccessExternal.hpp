#pragma once

#include <optional>
#include <random>

#include <Usagi/Entity/Archetype.hpp>
#include <Usagi/Entity/ComponentQueryFilter.hpp>

#include "ComponentAccessReadOnly.hpp"
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
    using EntityViewT       =
        typename DatabaseT::template EntityUserViewT<ComponentAccess>;

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

    auto entity(EntityId id)
    {
        return EntityView<DatabaseT, ComponentAccessT>(
            this->mDatabase,
            id.page,
            id.offset
        );
    }

    /**
     * \brief Returns a read-only view on all allocated entity pages. Beware
     * that this view also visits uncreated entities in pages.
     * BUG: unfiltered_view() can be used to obtain EntityId to entities that are not yet created. The obtained Id can be used with to create an EntityView and cause modifications to those uncreated entities.
     * \return
     */
    auto unfiltered_view()
    {
        return EntityDatabaseViewUnfiltered<
            DatabaseT,
            ComponentAccessReadOnly
        >(this->mDatabase);
    }

    /**
     * \brief Derive a view on the entity database based on the access traits
     * used to instantiate this access object.
     * \tparam Include
     * \tparam Exclude
     * \param include Leaving this field blank is effectively doing a
     * SELECT * including destroyed entities whose storage is not freed.
     * \param exclude
     * \return
     */
    template <Component... Include, Component... Exclude>
    auto view(
        ComponentFilter<Include...> include = { },
        // defaults to empty exclude mask via class template argument deduction
        ComponentFilter<Exclude...> exclude = { })
        requires (CanReadComponentsByFilter<ComponentAccess, decltype(include)>
            && CanReadComponentsByFilter<ComponentAccess, decltype(exclude)>)
    {
        return EntityDatabaseViewFiltered<
            DatabaseT,
            ComponentAccessT,
            decltype(include),
            decltype(exclude)
        >(this->mDatabase);
    }

    template <SimpleComponentQuery Q>
    decltype(auto) view(Q query)
    {
        return view(typename Q::IncludeFilter(), typename Q::ExcludeFilter());
    }

    // todo parallel evaluation?
    template <SimpleComponentQuery Q>
    std::size_t count(Q query) const
    {
        return std::ranges::distance(view(std::move(query)));
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
        requires (CanReadComponentsByFilter<ComponentAccess, decltype(include)>
            && CanReadComponentsByFilter<ComponentAccess, decltype(exclude)>)
    {
        return EntityPageViewFiltered<
            DatabaseT,
            ComponentAccessT,
            decltype(include),
            decltype(exclude)
        >(this->mDatabase, page_idx);
    }

    template <Component... InitialComponents>
    decltype(auto) insert(Archetype<InitialComponents...> &archetype)
        requires (... && CanWriteComponent<ComponentAccessT, InitialComponents>)
    {
        return this->mDatabase->template entity_view<ComponentAccess>(
            this->mDatabase->insert(archetype)
        );
    }

    struct SamplingEventCounter
    {
        std::size_t num_invalid = 0;
        std::size_t num_include_unsatisfied = 0;
        std::size_t num_exclude_unsatisfied = 0;

        auto & operator+=(const SamplingEventCounter &rhs)
        {
            num_invalid += rhs.num_invalid;
            num_include_unsatisfied += rhs.num_include_unsatisfied;
            num_exclude_unsatisfied += rhs.num_exclude_unsatisfied;

            return *this;
        }
    };

    auto create_sampling_counter() const
    {
        return SamplingEventCounter { };
    }

    // todo: should newly inserted entities be visible to sampling process?
    template <Component... Include, Component... Exclude>
    std::optional<EntityViewT> sample(
        auto &&rng,
        ComponentFilter<Include...> include = { },
        ComponentFilter<Exclude...> exclude = { },
        const std::size_t limit = DatabaseT::ENTITY_PAGE_SIZE,
        SamplingEventCounter *insights = nullptr)
        requires (CanReadComponentsByFilter<ComponentAccess, decltype(include)>
            && CanReadComponentsByFilter<ComponentAccess, decltype(exclude)>)
    {
        // todo: hit rate may drop rapidly when page storage usage is sparse. fallback to reservoir sampling?
        // todo: validate distribution
        // todo: avoid destroyed entities
        std::uniform_int_distribution<std::size_t> dist {
            0, this->entity_pages().size() * DatabaseT::ENTITY_PAGE_SIZE
        };
        SamplingEventCounter counter;
        // limit the number of attempts since it's possible for the database
        // to be completely empty.
        for(std::size_t i = 0; i < limit; ++i)
        {
            const auto seq = dist(rng);
            const auto page = seq / DatabaseT::ENTITY_PAGE_SIZE;
            const auto offset = seq % DatabaseT::ENTITY_PAGE_SIZE;

            EntityViewT view { this->mDatabase, page, offset };
            // rejection sampling
            if(!view.valid())
            {
                ++counter.num_invalid;
                continue;
            }
            if(!view.include(include))
            {
                ++counter.num_include_unsatisfied;
                continue;
            }
            if(!view.exclude(exclude))
            {
                ++counter.num_exclude_unsatisfied;
                continue;
            }
            if(insights) *insights = counter;
            return view;
        }
        return { };
    }
};
}
