#pragma once

#include <tuple>
#include <array>

#include <Usagi/Game/detail/ComponentAccessAllowAll.hpp>
#include <Usagi/Game/detail/ComponentFilter.hpp>
#include <Usagi/Game/detail/EntityId.hpp>
#include <Usagi/Game/detail/EntityIterator.hpp>
#include <Usagi/Game/detail/EntityPage.hpp>
#include <Usagi/Game/detail/EntityPageIterator.hpp>
#include <Usagi/Game/Entity/Archetype.hpp>
#include <Usagi/Library/Memory/PoolAllocator.hpp>
#include <Usagi/Library/Memory/LockGuard.hpp>

namespace usagi
{
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
 */
template <
    typename EnabledComponents = ComponentFilter<>,
    typename Storage = PoolAllocator<int>
>
class EntityDatabase
{
public:
    // Should only be friend with the access with the same type of database,
    // but C++ does not allow being friend with partial class template
    // specializations.
    template <typename Database>
    friend class EntityDatabaseAccessInternal;

    template <typename Database>
    friend class EntityPageIterator;

private:
    template<typename T>
    using StorageT = typename Storage::template rebind<T>;

public:
    using ComponentFilterT      = EnabledComponents;
    using EntityPageT           =
        typename EnabledComponents::template Apply<EntityPage>;
    using EntityPageAllocatorT  = StorageT<EntityPageT>;
    using EntityPageIteratorT   = EntityPageIterator<EntityDatabase>;
    using EntityUserViewT = EntityView<EntityDatabase, ComponentAccessAllowAll>;

    constexpr static std::size_t ENTITY_PAGE_SIZE = EntityPageT::PAGE_SIZE;

private:
    std::uint64_t           mLastEntityId = 0;
    std::size_t             mFirstEntityPageIndex = EntityPageT::INVALID_PAGE;
    EntityPageAllocatorT    mEntityPages;
    // todo lockless
    SpinLock                mEntityPageAllocationLock;

    template <Component C>
    using SingleComponentStorageT = StorageT<std::array<C, ENTITY_PAGE_SIZE>>;

    using ComponentStorageT =
        typename EnabledComponents::template NestedApply<
            std::tuple,
            SingleComponentStorageT
        >;
    ComponentStorageT mComponentStorage;

    struct EntityPageInfo
    {
        std::size_t index = -1;
        EntityPageT *ptr = nullptr;
    };

    EntityPageInfo allocateEntityPage()
    {
        std::size_t page_idx = mEntityPages.allocate();
        EntityPageT &page = mEntityPages.at(page_idx);

        std::allocator_traits<EntityPageAllocatorT>::construct(
            mEntityPages, &page
        );

        {
            LockGuard lock(mEntityPageAllocationLock);

            page.first_entity_id = mLastEntityId;
            // Singly linked list of pages
            page.next_page = mFirstEntityPageIndex;

            mLastEntityId += ENTITY_PAGE_SIZE;
            mFirstEntityPageIndex = page_idx;
        }

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
     * \tparam InitialComponents
     * \param archetype
     * \return
     */
    template <Component... InitialComponents>
    EntityPageInfo tryReuseCoherentPage(
            Archetype<InitialComponents...> &archetype)
    {
        const auto page_idx = archetype.mLastUsedPageIndex;

        // If this is a new Archetype instance, return a new page
        if(page_idx == -1)
            return allocateEntityPage();

        // The page was deleted and the storage shrunk so the index refers to
        // invalid memory
        if(page_idx >= mEntityPages.size())
            return allocateEntityPage();

        auto ptr = &mEntityPages.at(page_idx);

        // The page got recycled and was made into a new page
        if(ptr->first_entity_id != archetype.mLastUsedPageInitialId)
            return allocateEntityPage();

        return EntityPageInfo {
            .index = page_idx,
            .ptr = ptr
        };
    }

    void reserve_entity_page_storage(const std::size_t size)
    {
        mEntityPages.init_storage(size);
    }

    template <template <Component...> typename Filter, Component... Cs>
    void reserve_component_storage(const std::size_t size, Filter<Cs...>)
    {
        (..., std::get<SingleComponentStorageT<Cs>>(mComponentStorage)
            .init_storage(size));
    }

public:
    /**
     * \brief
     * \param pool_mem_reserve_size Default = 2^32 entries for both Entity Page
     * Pool and Component Pools.
     */
    explicit EntityDatabase(
        const std::size_t pool_mem_reserve_size = 1ull << 32)
    {
        reserve_entity_page_storage(pool_mem_reserve_size);
        reserve_component_storage(pool_mem_reserve_size, ComponentFilterT());
    }

    template <typename ComponentAccess>
    auto createAccess()
    {
        return EntityDatabaseAccess<EntityDatabase, ComponentAccess>(
            *this
        );
    }

    auto entityView(const EntityId id)
    {
        return EntityUserViewT {
            this,
            id.page_id,
            id.id % ENTITY_PAGE_SIZE
        };
    }

    template <
        Component... InitialComponents,
        typename EntityIdOutputIterator
    >
    auto create(
        Archetype<InitialComponents...> &archetype,
        const std::size_t count,
        EntityIdOutputIterator id_output)
    {
        // Note that if you create entities from multiple threads,
        // the archetype must NOT be shared among the threads and ensure that
        // only one thread is able to access the page referenced by
        // the archetype.

        EntityPageInfo page = tryReuseCoherentPage(archetype);
        EntityId last_entity_id;

        for(std::size_t i = 0; i < count; ++i)
        {
            // The Entity Page is full, allocate a new one.
            // Note that the holes in an Entity Page is never used.
            // When the Page becomes empty, the Page will be recycled.
            if(page.ptr->first_unused_index == ENTITY_PAGE_SIZE)
            {
                page = allocateEntityPage();
            }

            // todo: insert data by components instead of entities

            EntityUserViewT view {
                this, page.ptr, page.ptr->first_unused_index
            };

            // Initialize components for the new entity
            (..., (view.template addComponent<InitialComponents>()
                = archetype.template val<InitialComponents>()));

            last_entity_id = EntityId {
                .page_id = page.index,
                .id = page.ptr->first_entity_id + page.ptr->first_unused_index
            };

            // If an output iterator is passed in, use it to write entity ids
            if constexpr (!std::is_same_v<decltype(id_output), std::nullptr_t>)
            {
                *id_output = last_entity_id;
                ++id_output;
            }

            // Mark the Entity slot as used
            ++page.ptr->first_unused_index;
            // Record Page index hint
            archetype.mLastUsedPageIndex = page.index;
            // The page may have been recycled and made into another page
            // so check the id range to make sure it was the original one.
            archetype.mLastUsedPageInitialId = page.ptr->first_entity_id;
        }

        return last_entity_id;
    }

    template <Component T>
    auto & componentStorage()
    {
        return std::get<SingleComponentStorageT<T>>(mComponentStorage);
    }

    /**
     * \brief Release unused pages and clear dirty flags of pages.
     */
    void reclaim_pages()
    {
        std::size_t cur = mFirstEntityPageIndex;
        std::size_t *prev_ptr = &mFirstEntityPageIndex;

        while(cur != EntityPageT::INVALID_PAGE)
        {
            bool drop_page = true;
            release_empty_component_pages(cur, ComponentFilterT(), drop_page);

            EntityPageT &page = mEntityPages.at(cur);

            if(drop_page)
            {
                // link previous page to the next page of current page
                *prev_ptr = page.next_page;
                // clear the id range so it doesn't get accidentally
                // recognized as a valid page
                page.first_entity_id = -1;
                // release page
                mEntityPages.deallocate(cur);
                // increment iteration position
                cur = *prev_ptr;
            }
            else
            {
                // if next page was to be freed, this is the pointer to be
                // updated.
                prev_ptr = &page.next_page;
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
        bool &drop_page)
    {
        (..., release_empty_component_page<C>(idx, drop_page));
    }

    // returns if the component page is empty
    template <Component C>
    void release_empty_component_page(const std::size_t e_idx, bool &drop_page)
    {
        EntityPageT &page = mEntityPages.at(e_idx);
        auto &c_idx = page.template component_page_index<C>();

        // Not allocated
        if(c_idx == EntityPageT::INVALID_PAGE)
            return;

        // Page in use
        if(page.template component_enabled_mask<C>() != 0)
        {
            drop_page = false;
            return;
        }

        // Free empty component page
        componentStorage<C>().deallocate(c_idx);
        c_idx = EntityPageT::INVALID_PAGE;
    }
};
}
