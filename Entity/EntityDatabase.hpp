#pragma once

#include <array>

#include <Usagi/Library/Memory/LockGuard.hpp>
#include <Usagi/Library/Memory/SpinLock.hpp>
#include <Usagi/Runtime/Memory/PagedStorage.hpp>

#include "Archetype.hpp"
#include "detail/ComponentAccessAllowAll.hpp"
#include "detail/ComponentFilter.hpp"
#include "detail/EntityId.hpp"
#include "detail/EntityIterator.hpp"
#include "detail/EntityPage.hpp"
#include "detail/EntityPageIterator.hpp"

namespace usagi
{
namespace detail::entity
{
template <
    template <typename T> typename Storage,
    Component C,
    Component... EnabledComponents
>
using ComponentStorageT = Storage<
    std::array<
        C,
        EntityPage<EnabledComponents...>::PAGE_SIZE
    >
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
 */
template <
    template <typename T> typename Storage,
    Component... EnabledComponents
>
class EntityDatabase
    // storage for entity pages
    : protected Storage<EntityPage<EnabledComponents...>>
    // storage for components
    , protected detail::entity::ComponentStorageT<
        Storage, EnabledComponents, EnabledComponents...
    >...
{
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
    using EntityPageAllocatorT  = Storage<EntityPageT>;
    using EntityPageIteratorT   = EntityPageIterator<EntityDatabase>;
    using EntityUserViewT = EntityView<EntityDatabase, ComponentAccessAllowAll>;

    constexpr static std::size_t ENTITY_PAGE_SIZE = EntityPageT::PAGE_SIZE;

    static_assert(Memcpyable<EntityPageT>);

protected:
    struct Meta
    {
        // Count the allocation of Entity Pages
        std::uint64_t entity_seq_id = 0;
        // A linked list of entities is maintained in the order of allocation
        std::uint64_t first_entity_page_idx = EntityPageT::INVALID_PAGE;
        std::uint64_t last_entity_page_idx = EntityPageT::INVALID_PAGE;
    } mMeta;
    // todo lockless
    SpinLock mEntityPageAllocationLock;

    auto & entity_pages()
    {
        return static_cast<EntityPageAllocatorT&>(*this);
    }

    template <Component T>
    auto & component_storage()
    {
        using namespace detail::entity;
        return static_cast<
            ComponentStorageT<Storage, T, EnabledComponents...>&
        >(*this);
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

        std::allocator_traits<EntityPageAllocatorT>::construct(
            entity_pages(), &page
        );

        {
            LockGuard lock(mEntityPageAllocationLock);

            page.page_seq_id = mMeta.entity_seq_id;

            // Insert the new page at the end of linked list so that the
            // entities are accessed in the incremental order of entity id
            // when being iterated

            // The linked list is empty and we are inserting the first page
            if(mMeta.first_entity_page_idx == EntityPageT::INVALID_PAGE)
            {
                assert(mMeta.last_entity_page_idx == EntityPageT::INVALID_PAGE);
                mMeta.first_entity_page_idx = page_idx;
                mMeta.last_entity_page_idx = page_idx;
            }
            // The linked list has at least one page
            else
            {
                assert(mMeta.last_entity_page_idx != EntityPageT::INVALID_PAGE);
                auto &last = entity_pages().at(mMeta.last_entity_page_idx);
                assert(last.next_page == EntityPageT::INVALID_PAGE);
                last.next_page = page_idx;
                mMeta.last_entity_page_idx = page_idx;
            }

            ++mMeta.entity_seq_id;
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

        return EntityPageInfo {
            .index = page_idx,
            .ptr = ptr
        };
    }

    /*
    template <
        typename Func,
        template <Component...> typename Filter,
        Component... Cs
    >
    void init_component_storage(Func &&func, Filter<Cs...>)
    {
        (..., func(std::get<SingleComponentStorageT<Cs>>(mComponentStorage)));
    }
    */

public:
    EntityDatabase() = default;

    /*
    template <typename Func>
    void init_entity_page_storage(Func &&func)
    {
        func(mEntityPages);
    }

    template <typename Func>
    void init_component_storage(Func &&func)
    {
        init_component_storage(std::forward<Func>(func), ComponentFilterT());
    }
    */

    /*
    EntityDatabaseMetaInfo get_state() const
    {
        EntityDatabaseMetaInfo meta;

        meta.entity_seq_id = mMeta.entity_seq_id;
        meta.first_entity_page_idx = mMeta.first_entity_page_idx;
        meta.last_entity_page_idx = mMeta.last_entity_page_idx;

        return meta;
    }

    void restore_state(const EntityDatabaseMetaInfo &meta)
    {
        assert(entity_pages().size() > meta.first_entity_page_idx);
        assert(entity_pages().size() > meta.last_entity_page_idx);

        mMeta.entity_seq_id = meta.entity_seq_id;
        mMeta.first_entity_page_idx = meta.first_entity_page_idx;
        mMeta.last_entity_page_idx = meta.last_entity_page_idx;
    }
    */

    template <typename ComponentAccess>
    auto create_access()
    {
        return EntityDatabaseAccess<EntityDatabase, ComponentAccess>(*this);
    }

    auto entity_view(const EntityId id)
    {
        return EntityUserViewT {
            this,
            &entity_pages().at(id.page),
            id.offset
        };
    }

    template <Component... InitialComponents>
    auto create(Archetype<InitialComponents...> &archetype)
    {
        // Note that if you create entities from multiple threads,
        // the archetype must NOT be shared among the threads and ensure that
        // only one thread is able to access the page referenced by
        // the archetype. Since entities are always created using archetypes
        // and allocated pages are bound with the archetypes, it's guaranteed
        // that no concurrent entity creation on one page will happen.

        EntityPageInfo page = try_reuse_coherent_page(archetype);

        // The Entity Page is full, allocate a new one.
        // Note that the holes in an Entity Page is never used.
        // When the Page becomes empty, the Page will be recycled.
        if(page.ptr->first_unused_index == ENTITY_PAGE_SIZE)
        {
            page = allocate_entity_page();
        }

        // todo: insert data by components instead of entities

        EntityUserViewT view {
            this, page.ptr, page.ptr->first_unused_index
        };

        // Initialize components for the new entity
        (..., (view.template add_component<InitialComponents>()
            = archetype.template val<InitialComponents>()));

        const EntityId entity_id = EntityId {
            .offset = page.ptr->first_unused_index,
            .page = page.index
        };

        // Mark the Entity slot as used
        ++page.ptr->first_unused_index;
        // Record Page index hint
        archetype.mLastUsedPageIndex = page.index;
        // The page may have been recycled and made into another page
        // so check the id range to make sure it was the original one.
        archetype.mLastUsedPageSeqId = page.ptr->page_seq_id;

        return entity_id;
    }

    /**
     * \brief Release unused pages and clear dirty flags of pages.
     */
    void reclaim_pages()
    {
        std::size_t cur = mMeta.first_entity_page_idx;
        std::size_t *prev_next_ptr = &mMeta.first_entity_page_idx;
        std::size_t prev = EntityPageT::INVALID_PAGE;

        while(cur != EntityPageT::INVALID_PAGE)
        {
            bool drop_page = true;
            release_empty_component_pages(cur, ComponentFilterT(), drop_page);

            EntityPageT &page = entity_pages().at(cur);

            if(drop_page)
            {
                // link the previous page to the next page of current page.
                // if this is the only page, this clears the linked list
                // by setting mMeta.first_entity_page_idx to INVALID_PAGE
                // because page.next_page would have that value.
                *prev_next_ptr = page.next_page;
                // this is the last page
                if(cur == mMeta.last_entity_page_idx)
                {
                    assert(page.next_page == EntityPageT::INVALID_PAGE);
                    // if this was the only page, the list becomes empty
                    mMeta.last_entity_page_idx = prev;
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
        bool &drop_page)
    {
        (..., release_empty_component_page<C>(idx, drop_page));
    }

    // returns if the component page is empty
    template <Component C>
    void release_empty_component_page(const std::size_t e_idx, bool &drop_page)
    {
        EntityPageT &page = entity_pages().at(e_idx);
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
        component_storage<C>().deallocate(c_idx);
        c_idx = EntityPageT::INVALID_PAGE;
    }
};

template <Component... EnabledComponents>
using EntityDatabaseInMemory = EntityDatabase<
    PagedStorageInMemory,
    EnabledComponents...
>;
}
