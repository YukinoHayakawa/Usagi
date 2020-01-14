#pragma once

#include <tuple>

#include <Usagi/Experimental/v2/Game/_detail/ComponentAccessAllowAll.hpp>
#include <Usagi/Experimental/v2/Game/_detail/ComponentMask.hpp>
#include <Usagi/Experimental/v2/Game/_detail/EntityIterator.hpp>
#include <Usagi/Experimental/v2/Game/_detail/EntityPage.hpp>
#include <Usagi/Experimental/v2/Game/Entity/Archetype.hpp>
#include <Usagi/Utility/Allocator/PoolAllocator.hpp>
#include <Usagi/Utility/ParameterPackIndex.hpp>

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
    friend class EntityDatabaseInternalAccess;

public:
    constexpr static std::size_t ENTITY_PAGE_SIZE = EntityPageSize;

    using EntityPageT       = EntityPage<
        ENTITY_PAGE_SIZE, EnabledComponents...
    >;
    using ComponentMaskT        = typename EntityPageT::ComponentMaskT;
    using EntityPageAllocatorT  = PoolAllocator<EntityPageT>;
    using EntityPageIteratorT   = typename EntityPageAllocatorT::IteratorT;

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

    /*
    template <
        Component... InitialComponents
    >
    EntityPageIteratorT findCoherentPage()
    {
        return
    }
    */

    EntityPageT * allocateEntityPage()
    {
        std::size_t page_idx = mEntityPages.allocate();
        EntityPageT &page = mEntityPages.block(page_idx);
        page.first_entity_id = mLastEntityId;
        mLastEntityId += EntityPageSize;
        return &page;
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

    /*
    template <
        typename ComponentAccess,
        typename... IncludeFilter,
        typename... ExcludeFilter
    >
    auto view(
        ComponentFilter<IncludeFilter...> include,
        ComponentFilter<ExcludeFilter...> exclude) const
    {
        return EntityDatabaseView<
            EntityDatabase,
            ComponentAccess,
            decltype(include),
            decltype(exclude)
        >();
    }
    */

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

    template <Component... InitialComponents>
    auto create(
        const Archetype<InitialComponents...> &archetype,
        const std::size_t count)
    {
        // todo locate a page which is likely to improve data coherence
        // auto &page = findCoherentPage<InitialComponents...>();

        EntityPageT *page = nullptr;

        for(std::size_t i = 0, j = 0; i < count; ++i)
        {
            if(j == 0)
                page = allocateEntityPage();

            // todo: insert data by components instead of entities

            EntityView<EntityDatabase, ComponentAccessAllowAll> view {
                this, page, j
            };

            ((view.template addComponent<InitialComponents>()
                = archetype.template initialValue<InitialComponents>()), ...);

            ++j;
            if(j == ENTITY_PAGE_SIZE)
                j = 0;
        }
    }

    template <Component T>
    auto & componentStorage()
    {
        return std::get<ComponentStorageT<T>>(mComponentStorage);
    }
};
}
