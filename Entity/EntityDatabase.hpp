#pragma once

#include <array>
#include <mutex>

#include <Usagi/Runtime/ErrorHandling.hpp>
#include <Usagi/Runtime/Memory/PagedStorage.hpp>

#include "Archetype.hpp"
#include "detail/ComponentAccessAllowAll.hpp"
#include "detail/ComponentFilter.hpp"
#include "detail/EntityDatabaseAccessExternal.hpp"
#include "detail/EntityId.hpp"
#include "detail/EntityIterator.hpp"
#include "detail/EntityPage.hpp"
#include "detail/EntityPageIterator.hpp"

namespace usagi
{
namespace entity
{
// These policies are declared as tag classes to avoid the problem of having
// an integer in the mangled type of instantiated database type. Such an
// integer makes it harder when we want to inject database access type into
// JIT codes.
namespace insertion_policy
{
// Use the first empty slot find in the entity database. Efficient in
// memory consumption but no data coherence is considered. Suitable when
// data are highly homogeneous and entity removal pattern is random.
struct FirstVacancy;

// Reuse the previously used page referred by the archetype. Removed
// entities will not be reused. Suitable when entity removal pattern is
// nearly FIFO and entities has to be iterated in the order of insertion
// (by archetype).
struct ReuseArchetypePage;
}

template <
    template <typename T> typename Storage = PagedStorageInMemory,
    typename InsertionPolicy = insertion_policy::ReuseArchetypePage
>
struct EntityDatabaseConfiguration
{
    template <typename T>
    using StorageT = Storage<T>;

    using InsertionPolicyT = InsertionPolicy;
};
}

namespace detail::entity
{
template <
    template <typename T> typename Storage,
    Component C
>
using ComponentStorageT = std::conditional_t<
    TagComponent<C>,
    Tag<C>, // an empty struct for dispatching purpose only
    Storage<std::array<C, EntityPageMeta::PAGE_SIZE>>
>;
}

/**
 * \brief Entity Database manages Entities and their Component data. It
 * provides compile time access permission validation for the Executive
 * to prevent Systems from violating the data dependency.
 *
 * Entity Database consists of Entity Pages and Component Pages.
 * The pages are stored in linear containers with internal pooling mechanism.
 * This arrangement makes it possible to directly dump and load the binary
 * data of the pages without any further processing. It is also possible to
 * map the memory onto disk files.
 *
 * \tparam EnabledComponents List of allowed component types. There shall not
 * be two identical types in the list.
 *
 * todo implement interfaces & implementations using mixins
 */
template <
    typename Config,
    Component... EnabledComponents
>
class EntityDatabase
    // storage for entity pages
    : protected Config::template StorageT<EntityPage<EnabledComponents...>>
    // storage for components
    , protected detail::entity::ComponentStorageT<
        Config::template StorageT, EnabledComponents
    >...
{
    template <typename T>
    using StorageT = typename Config::template StorageT<T>;
    using InsertionPolicyT = typename Config::InsertionPolicyT;

public:
    // Should only be friend with the access with the same type of database,
    // but C++ does not allow being friend with partial class template
    // specializations.
    template <typename Database>
    friend class EntityDatabaseAccessInternal;

    template <typename Database>
    friend class EntityPageIterator;

    using ComponentFilterT      = ComponentFilter<EnabledComponents...>;
    using EntityPageT           = EntityPage<EnabledComponents...>;
    using EntityIndexT          = typename EntityPageT::EntityIndexT;
    using EntityArrayT          = typename EntityPageT::EntityArrayT;
    using EntityPageStorageT    = StorageT<EntityPageT>;
    using EntityPageIteratorT   = EntityPageIterator<EntityDatabase>;
    template <typename ComponentAccess>
    using EntityUserViewT = EntityView<EntityDatabase, ComponentAccess>;

    constexpr static std::size_t ENTITY_PAGE_SIZE = EntityPageT::PAGE_SIZE;

protected:
    // default metadata storage. may be overriden.
    struct Meta
    {
        // Count the allocation of Entity Pages
        std::uint64_t entity_seq_id = 0;
        // A linked list of entities is maintained in the order of allocation
        std::uint64_t first_entity_page_idx = EntityPageT::INVALID_PAGE;
        std::uint64_t last_entity_page_idx = EntityPageT::INVALID_PAGE;
    } mDefaultMeta;

    // todo perf. better solution? use traits?
    // enable the metadata to be stored in memory mapped area so it doesn't
    // have to be manually saved
    virtual Meta & header()
    {
        return mDefaultMeta;
    }

    std::mutex mEntityPageAllocationLock;

    EntityPageStorageT & entity_pages()
    {
        return *this;
    }

    template <Component C>
    auto component_storage() -> detail::entity::ComponentStorageT<StorageT, C> &
        requires (!TagComponent<C>)
    {
        return *this;
    }

    struct EntityPageInfo
    {
        std::size_t index = -1;
        EntityPageT *ptr = nullptr;
    };

    EntityPageInfo allocate_entity_page()
    {
        std::size_t page_idx = entity_pages().allocate();
        EntityPageT &page = entity_pages().at(page_idx);

        std::allocator_traits<EntityPageStorageT>::construct(
            entity_pages(), &page
        );

        std::unique_lock lock(mEntityPageAllocationLock);

        page.page_seq_id = header().entity_seq_id;

        // Insert the new page at the end of linked list so that the
        // entities are accessed in the incremental order of entity id
        // when being iterated

        // The linked list is empty and we are inserting the first page
        if(header().first_entity_page_idx == EntityPageT::INVALID_PAGE)
        {
            assert(header().last_entity_page_idx == EntityPageT::INVALID_PAGE);
            header().first_entity_page_idx = page_idx;
            header().last_entity_page_idx = page_idx;
        }
        // The linked list has at least one page
        else
        {
            assert(header().last_entity_page_idx != EntityPageT::INVALID_PAGE);
            auto &last = entity_pages().at(header().last_entity_page_idx);
            assert(last.next_page == EntityPageT::INVALID_PAGE);
            last.next_page = page_idx;
            header().last_entity_page_idx = page_idx;
        }

        ++header().entity_seq_id;

        return EntityPageInfo {
            .index = page_idx,
            .ptr = &page
        };
    }

    /**
     * \brief Try to reuse the most recent page used to store the same
     * Archetype. If the page is full or invalid, return a new page. The
     * archetype should be kept by the System.
     *
     * The rationale of this function can help in improving iteration
     * efficiency is:
     *
     * Assuming that Systems are reasonably modularized, then a System
     * should be producing a similar kind of Entity all the time. Therefore,
     * these Entities are likely to experience similar kind of manipulations,
     * thus having a higher change of having a similar set of components
     * during their lifetimes. Therefore, putting these Entities together
     * may improve overall iteration and cache efficiency.
     *
     * To achieve the objective of putting Entities created from the same
     * Archetype together, there need some way of finding the last page used
     * to create the Entity of the same kind by examining the Archetype type.
     * One obvious approach is using a hash map to map the Archetype type to
     * a page index. However, this may impose higher overhead during runtime
     * than necessary.
     *
     * Instead, the information of the previous page used for one particular
     * Archetype may be stored in the Archetype itself as a hint. Assuming that
     * the System is alive for a reasonable length of time, it will gradually
     * fill in the Page. If it does not, the cost is merely some waste of
     * memory and accessing a random page with only one Entity. Meanwhile,
     * if two or more Systems have the same type of Archetype instances and
     * assume that the previous point holds, the Page utilization should be
     * reasonably good.
     *
     * Bug: Worst case space complexity is 32x, which could be observed in scenarios where Entities are destroyed in a random manner, such as genetic algorithms. Consider inventing a better mechanism.
     *
     * \tparam InitialComponents
     * \param archetype
     * \return
     */
    template <Component... InitialComponents>
    EntityPageInfo try_reuse_coherent_page(
        Archetype<InitialComponents...> &archetype)
    {
        const auto page_idx = archetype.mLastUsedPageIndex;

        // If this is a new Archetype instance, return a new page
        if(page_idx == -1)
            return allocate_entity_page();

        // The page was deleted and the storage shrunk so the index refers to
        // invalid memory
        if(page_idx >= entity_pages().size())
            return allocate_entity_page();

        auto ptr = &entity_pages().at(page_idx);

        // The page got recycled and was made into a new page
        if(ptr->page_seq_id != archetype.mLastUsedPageSeqId)
            return allocate_entity_page();

        return { .index = page_idx, .ptr = ptr };
    }

    EntityPageInfo first_page_vacancy()
    {
        EntityDatabaseAccessExternal<
            EntityDatabase, ComponentAccessReadOnly
        > range { this };

        auto begin = range.begin();
        auto end = range.end();

        while(begin != end)
        {
            if(begin->free_mask != 0)
                break;
            ++begin;
        }

        if(begin == end)
            return allocate_entity_page();

        return { .index = begin.index(), .ptr = &begin.ref() };
    }

public:
    EntityDatabase() = default;
    virtual ~EntityDatabase() = default;

    template <typename ComponentAccess>
    auto create_access()
    {
        return EntityDatabaseAccessExternal<
            EntityDatabase, ComponentAccess
        >(this);
    }

    template <
        typename ComponentAccess,
        typename ViewT = EntityUserViewT<ComponentAccess>
    >
    ViewT entity_view(const EntityId id)
    {
        return {
            this,
            id.page,
            static_cast<typename ViewT::EntityIndexT>(id.offset)
        };
    }

    template <Component... InitialComponents>
    auto insert(Archetype<InitialComponents...> &archetype)
    {
        // Note that if you create entities from multiple threads,
        // the archetype must NOT be shared among the threads and ensure that
        // only one thread is able to access the page referenced by
        // the archetype. Since entities are always created using archetypes
        // and allocated pages are bound with the archetypes, it's guaranteed
        // that no concurrent entity creation on one page will happen.

        EntityPageInfo page;

        using namespace entity::insertion_policy;

        // bug: this is a temporary hack to the worst space complexity faced by current applications. component coherence must be carefully addressed later.
        if constexpr(std::is_same_v<InsertionPolicyT, FirstVacancy>)
            page = first_page_vacancy();
        else if constexpr(std::is_same_v<InsertionPolicyT, ReuseArchetypePage>)
            page = try_reuse_coherent_page(archetype);
        else
            // Always fails, but since its a dependent expression it won't
            // cause the program to be ill-formed.
            // https://stackoverflow.com/questions/38304847/constexpr-if-and-static-assert
            static_assert(!sizeof(EntityDatabase), "Invalid insertion policy.");

        // EntityPageInfo
        EntityIndexT inner_index;

        // Try to allocate an empty slot from the entity page.
        // If the Entity Page is full, allocate a new one.
        if((inner_index = page.ptr->allocate()) == ENTITY_PAGE_SIZE)
        {
            page = allocate_entity_page();
            inner_index = page.ptr->allocate();
        }
        assert(inner_index < ENTITY_PAGE_SIZE);

        // Record Page index hint
        archetype.mLastUsedPageIndex = page.index;
        // It's possible that an entity page referenced by an archetype to
        // be recycled before next use. This sequential ID is assigned to
        // detect such cases.
        archetype.mLastUsedPageSeqId = page.ptr->page_seq_id;

        EntityUserViewT<ComponentAccessAllowAll> view {
            this, page.index, inner_index
        };

        // todo: insert data by components instead of entities
        //
        // Initialize components for the new entity
        (..., (
            view.template add_component<InitialComponents>() =
                archetype.template component<InitialComponents>()
        ));

        const EntityId entity_id {
            .offset = inner_index,
            .page = page.index
        };

        return entity_id;
    }

    /**
     * \brief Release unused pages and clear dirty flags of pages.
     * Note: this is meant to be used when all other operations are finished.
     * Synchronization is performed externally.
     */
    void reclaim_pages()
    {
        std::size_t cur = header().first_entity_page_idx;
        std::size_t *prev_next_ptr = &header().first_entity_page_idx;
        std::size_t prev = EntityPageT::INVALID_PAGE;

        while(cur != EntityPageT::INVALID_PAGE)
        {
            EntityArrayT usage_mask = 0;
            release_empty_component_pages(cur, ComponentFilterT(), usage_mask);

            EntityPageT &page = entity_pages().at(cur);
            page.free_mask = ~usage_mask;

            if(usage_mask == 0)
            {
                // link the previous page to the next page of current page.
                // if this is the only page, this clears the linked list
                // by setting mMeta.first_entity_page_idx to INVALID_PAGE
                // because page.next_page would have that value.
                *prev_next_ptr = page.next_page;
                // this is the last page
                if(cur == header().last_entity_page_idx)
                {
                    assert(page.next_page == EntityPageT::INVALID_PAGE);
                    // if this was the only page, the list becomes empty
                    header().last_entity_page_idx = prev;
                }
                // clear the id range so it doesn't get accidentally
                // recognized as a valid page
                page.page_seq_id = -1;
                // release page
                entity_pages().deallocate(cur);
                // increment iteration position.
                // prev_ptr always points to a valid memory position even
                // if the list was empty. when that is the case, it just
                // points to mMeta.first_entity_page_idx.
                cur = *prev_next_ptr;
                // in this case the prev index doesn't have to updated
            }
            else
            {
                // if next page was to be freed, this is the pointer to be
                // updated.
                prev_next_ptr = &page.next_page;
                prev = cur;
                cur = page.next_page;
            }
        }
    }

private:
    // returns if the page is entirely empty so it can be released
    template <Component... C>
    void release_empty_component_pages(
        const std::size_t idx,
        ComponentFilter<C...>,
        EntityArrayT &usage_mask)
    {
        (..., release_empty_component_page<C>(idx, usage_mask));
    }

    // returns if the component page is empty
    template <Component C>
    void release_empty_component_page(
        const std::size_t e_idx,
        EntityArrayT &usage_mask)
    {
        EntityPageT &page = entity_pages().at(e_idx);
        auto &c_idx = page.template component_page_index<C>();

        const auto c_page_unallocated = c_idx == EntityPageT::INVALID_PAGE;
        if constexpr(TagComponent<C>)
        {
            assert(c_page_unallocated);
        }
        else if(c_page_unallocated)
        {
            return;
        }

        const auto mask = page.template component_enabled_mask<C>();
        usage_mask |= mask;

        // Component page in use
        if(mask != 0)
        {
            return;
        }

        if constexpr(!TagComponent<C>)
        {
            // Free empty component page
            component_storage<C>().deallocate(c_idx);
            c_idx = EntityPageT::INVALID_PAGE;
        }
    }
};

template <
    Component... EnabledComponents
>
using EntityDatabaseDefaultConfig = EntityDatabase<
    entity::EntityDatabaseConfiguration<>,
    EnabledComponents...
>;
}
