#pragma once

#include <tuple>

#include <Usagi/Experimental/v2/Game/_details/ComponentMask.hpp>
#include <Usagi/Experimental/v2/Game/_details/EntityPage.hpp>
#include <Usagi/Experimental/v2/Game/_details/PermissionValidatorAllowAll.hpp>
#include <Usagi/Experimental/v2/Game/_details/EntityIterator.hpp>
#include <Usagi/Utility/Allocator/PoolAllocator.hpp>

namespace usagi
{
/**
 * \brief
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
    // Should only be friend with the iterators with the same type of database,
    // but C++ does not allow being friend with partial class template
    // specializations.
    template <
        typename Database,
        typename PermissionValidator
    >
    friend class EntityIterator;

    template <
        typename Database,
        typename PermissionValidator
    >
    friend class EntityDatabaseAccess;

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

    template <Component C>
    struct ComponentMaskBit
    {
        ComponentMaskT mask = 0;
    };

    std::tuple<ComponentMaskBit<EnabledComponents>...> mComponentMaskBits;

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
    EntityDatabase()
    {
        auto i = 0;
        ((std::get<ComponentMaskBit<EnabledComponents>>(mComponentMaskBits)
            .mask = 1ull << (i++)), ...);
    }

    template <Component C>
    ComponentMaskT componentMaskBit() const
    {
        return std::get<ComponentMaskBit<C>>(mComponentMaskBits).mask;
    }

    template <Component... Components>
    ComponentMaskT buildComponentMask() const
    {
        ComponentMaskT mask = 0;
        ((mask |= componentMaskBit<Components>()), ...);
        return mask;
    }

    /*
    template <
        typename PermissionValidator,
        typename... IncludeFilter,
        typename... ExcludeFilter
    >
    auto view(
        ComponentFilter<IncludeFilter...> include,
        ComponentFilter<ExcludeFilter...> exclude) const
    {
        return EntityDatabaseView<
            EntityDatabase,
            PermissionValidator,
            decltype(include),
            decltype(exclude)
        >();
    }
    */

    template <
        typename PermissionValidator
    >
    auto createAccess()
    {
        return EntityDatabaseAccess<EntityDatabase, PermissionValidator>(
            *this
        );
    }

    // void markEntityDirty(const EntityId &id)
    // {
    //     // todo
    // }

    // todo: move to DbAccess?
    template <
        Component... InitialComponents
    >
    void create(
        Archetype<InitialComponents...> &archetype,
        const std::size_t count = 1)
    {
        // todo locate a page which is likely to improve data coherence
        // auto &page = findCoherentPage<InitialComponents...>();

        EntityPageT *page = nullptr;

        for(std::size_t i = 0, j = 0; i < count; ++i)
        {
            if(j == 0)
                page = allocateEntityPage();

            // todo: insert data by components instead of entities

            EntityView<EntityDatabase, PermissionValidatorAllowAll> view {
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
