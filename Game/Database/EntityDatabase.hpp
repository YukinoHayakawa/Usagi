#pragma once

#include <tuple>
#include <bitset>
#include <limits>

#include <Usagi/Utility/Allocator/PoolAllocator.hpp>

#include <Usagi/Experimental/v2/ECS/Entity/Entity.hpp>
#include "details/EntityPage.hpp"
#include "details/ComponentMask.hpp"
#include "details/AllowAllPermissionChecker.hpp"
#include "details/EntityDatabaseView.hpp"

namespace usagi::ecs
{
/**
 * \brief
 * \tparam EntityPageSize Number of entities stored in each page. The
 * components storages will allocate components in the same unit.
 * \tparam EnabledComponents List of allowed component types. If there are
 * two identical types, the behavior depends on std::get().
 */
template <
    std::size_t EntityPageSize,
    Component... EnabledComponents
>
class EntityDatabase
{
    // Should only be friend with the iterators with the same type of database,
    // but C++ does not allow being friend with partial class template
    // specializations.
    template <
        typename Database,
        typename PermissionChecker
    >
    friend class EntityIterator;

public:
    using ComponentMaskT = ComponentMask<EnabledComponents...>;
    constexpr static std::size_t ENTITY_PAGE_SIZE = EntityPageSize;

private:
    PoolAllocator<EntityPage<
        ENTITY_PAGE_SIZE, EnabledComponents...
    >> mEntityPages;
    /**
     * \brief Caches the disjunction of component flags in each page for
     * faster page filtering.
     */
    // std::vector<ComponentMaskT> mPageDisjunctionMasks;
    // std::vector<bool> mPageDisjunctionMaskDirtyFlags;

    std::tuple<PoolAllocator<
        std::array<EnabledComponents, ENTITY_PAGE_SIZE>
    >...> mComponentStorage;

    template <Component C>
    struct ComponentMaskBit
    {
        ComponentMaskT mask = 0;
    };

    std::tuple<ComponentMaskBit<EnabledComponents>...> mComponentMaskBits;

public:
    EntityDatabase()
    {
        int i = 0;
        ((std::get<ComponentMaskBit<EnabledComponents>>(mComponentMaskBits)
            .mask = 1 << (i++)), ...);
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

    template <typename... IncludeFilter, typename... ExcludeFilter>
    auto view(
        ComponentFilter<IncludeFilter...> include,
        ComponentFilter<ExcludeFilter...> exclude) const
    {
        using PermissionChecker = AllowAllPermissionChecker;
        return EntityDatabaseView<
            EntityDatabase,
            PermissionChecker,
            decltype(include),
            decltype(exclude)
        >();
    }

    // void markEntityDirty(const EntityId &id)
    // {
    //     // todo
    // }

    template <Component... InitialComponents>
    EntityRange create(
        Archetype<InitialComponents...> &archetype,
        const std::size_t count = 1)
    {
        // locate a page which is likely to improve data coherence
        auto &page = findCoherentPage<InitialComponents...>();
        for(std::size_t i = 0; i < count; ++i)
        {
            // todo: insert data by components instead of entities
            auto &e = page.create();
            (e.addComponent<InitialComponents>()
                = archetype.initialValue<InitialComponents>(), ...);
        }
    }

    template <Component T>
    auto & componentStorage()
    {
        return std::get<PagedPool<T>>(mComponentStorage);
    }
};
}
