#pragma once

#include <tuple>

#include <Usagi/Experimental/v2/Game/_detail/ComponentAccessAllowAll.hpp>
#include <Usagi/Experimental/v2/Game/_detail/ComponentMask.hpp>
#include <Usagi/Experimental/v2/Game/_detail/EntityIterator.hpp>
#include <Usagi/Experimental/v2/Game/_detail/EntityPage.hpp>
#include <Usagi/Experimental/v2/Game/Entity/Archetype.hpp>
#include <Usagi/Utility/Allocator/PoolAllocator.hpp>
#include <Usagi/Utility/ParameterPackIndex.hpp>
#include <Usagi/Experimental/v2/Game/_detail/EntityId.hpp>

namespace usagi
{
/**
 * \brief Entity Database manages Entities and their Component data. It
 * provides compile time access permission validation for the Executive
 * to prevent Systems from violating the data dependency.
 *
 * Entity Database consists of Entity Pages and Component Pages, each
 * allocated in the unit specified by EntityPageSize template parameter.
 * The pages are stored in linear containers with internal pooling mechanism.
 * This arrangement makes it possible to directly dump and load the binary
 * data of the pages without any further processing. It is also possible to
 * map the memory onto disk files.
 *
 * \tparam EntityPageSize Number of entities stored in each page. The
 * components storage will allocate components in the same unit.
 * \tparam EnabledComponents List of allowed component types. If there are
 * two identical types, the behavior depends on std::get().
 */
template <
    std::uint16_t   EntityPageSize,
    Component...    EnabledComponents
>
class EntityDatabase
{
    // Should only be friend with the access with the same type of database,
    // but C++ does not allow being friend with partial class template
    // specializations.
    template <
        typename Database
    >
    friend class EntityDatabaseAccessInternal;

public:
    constexpr static std::size_t ENTITY_PAGE_SIZE = EntityPageSize;

    using EntityPageT       = EntityPage<
        ENTITY_PAGE_SIZE, EnabledComponents...
    >;
    using ComponentMaskT        = typename EntityPageT::ComponentMaskT;
    using EntityPageAllocatorT  = PoolAllocator<EntityPageT>;
    using EntityPageIteratorT   = typename EntityPageAllocatorT::IteratorT;
    using EntityUserViewT = EntityView<EntityDatabase, ComponentAccessAllowAll>;

private:
    std::uint64_t mLastEntityId = 0;
    EntityPageAllocatorT mEntityPages;

    /**
     * \brief Caches the disjunction of component flags in each page for
     * faster page filtering.
     */
    // std::vector<ComponentMaskT> mPageDisjunctionMasks;
    // std::vector<bool> mPageDisjunctionMaskDirtyFlags;

    template <Component C>
    using ComponentStorageT = PoolAllocator<
        std::array<C, ENTITY_PAGE_SIZE>
    >;

    std::tuple<ComponentStorageT<EnabledComponents>...> mComponentStorage;

    template <Component... InitialComponents>
    EntityPageIteratorT findCoherentPage()
    {
        // todo
    }

    struct EntityPageInfo
    {
        std::size_t index = -1;
        EntityPageT *ptr = nullptr;
    };

    EntityPageInfo allocateEntityPage()
    {
        std::size_t page_idx = mEntityPages.allocate();
        EntityPageT &page = mEntityPages.block(page_idx);

        page.first_entity_id = mLastEntityId;
        mLastEntityId += EntityPageSize;

        return EntityPageInfo {
            .index = page_idx,
            .ptr = &page
        };
    }

public:
    EntityDatabase() = default;

    template <Component C>
    static constexpr ComponentMaskT componentMaskBit()
    {
        ComponentMaskT mask;
        mask.set(ParameterPackIndex_v<C, EnabledComponents...>, 1);
        // static_assert(mask.any());
        return mask;
    }

    template <Component... Components>
    static constexpr ComponentMaskT buildComponentMask()
    {
        constexpr auto mask = (ComponentMaskT(0) | ... |
            componentMaskBit<Components>());
        // static_assert(mask.any());
        return mask;
    }

    template <typename ComponentAccess>
    auto createAccess()
    {
        return EntityDatabaseAccess<EntityDatabase, ComponentAccess>(
            *this
        );
    }

    // void markEntityDirty(const EntityId &id)
    // {
    //     // todo
    // }
    //

    auto entityView(const EntityId id)
    {
        return EntityUserViewT {
            this,
            id.page_id,
            id.id % ENTITY_PAGE_SIZE
        };
    }

    template <Component... InitialComponents>
    auto create(
        const Archetype<InitialComponents...> &archetype,
        const std::size_t count,
        EntityId *id_output)
    {
        // todo locate a page which is likely to improve data coherence
        // auto &page = findCoherentPage<InitialComponents...>();

        EntityPageInfo page;
        EntityId last_entity_id;

        for(std::size_t i = 0, j = 0; i < count; ++i)
        {
            if(j == 0)
                page = allocateEntityPage();

            // todo: insert data by components instead of entities

            EntityUserViewT view {
                this, page.ptr, j
            };

            ((view.template addComponent<InitialComponents>()
                = archetype.template initialValue<InitialComponents>()), ...);

            last_entity_id = EntityId {
                .page_id = page.index,
                .id = page.ptr->first_entity_id + j
            };

            if(id_output)
            {
                *id_output = last_entity_id;
                ++id_output;
            }

            ++j;
            if(j == ENTITY_PAGE_SIZE)
                j = 0;
        }

        return last_entity_id;
    }

    template <Component T>
    auto & componentStorage()
    {
        return std::get<ComponentStorageT<T>>(mComponentStorage);
    }
};
}
