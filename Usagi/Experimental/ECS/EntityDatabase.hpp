#pragma once

#include <tuple>
#include <limits>

#include <Usagi/Utility/Allocator/PoolAllocator.hpp>

#include "Component.hpp"

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
    // static_assert(sizeof...(EnabledComponents) <=
    //     std::numeric_limits<ComponentMask>::digits);

public:
    using ComponentMask = std::bitset<sizeof...(EnabledComponents)>;

private:
    PoolAllocator<EntityPage<
        EntityPageSize, EnabledComponents...
    >> mEntityPages;

    std::tuple<PoolAllocator<
        std::array<EnabledComponents, EntityPageSize>
    >...> mComponentStorage;

    template <Component C>
    struct ComponentMaskBit
    {
        ComponentMask mask = 0;
    };

    std::tuple<ComponentMaskBit<EnabledComponents>...> mComponentMaskBits;

public:
    using EntityT = Entity<EnabledComponents...>;

    EntityDatabase()
    {
        int i = 0;
        (std::get<ComponentMaskBit<EnabledComponents>>(
            mComponentMaskBits
        ).mask = 1 << (i++), ...);
    }

    template <Component C>
    ComponentMask componentMaskBit() const
    {
        return std::get<ComponentMaskBit<C>>(mComponentMaskBits).mask;
    }

    template <Component... Components>
    ComponentMask buildComponentMask() const
    {
        ComponentMask mask = 0;
        (mask |= componentMaskBit<Components>(), ...);
        return mask;
    }

    template <ComponentFilter Included, ComponentFilter Excluded>
    auto view() const
    {

    }

    template <Component... InitialComponents>
    EntityT create(Archetype<InitialComponents...> &archetype)
    {

    }

    class EntityRange
    {
        EntityId mBegin;
        EntityId mEnd;

    public:
        SequentialEntityIterator begin();
        SequentialEntityIterator end();
    };

    template <Component... InitialComponents>
    EntityRange create(
        Archetype<InitialComponents...> &archetype,
        const std::size_t count)
    {
        // locate a page which is likely to improve data coherence
        auto &page = findCoherentPage<InitialComponents...>();
        page.
    }

    template <Component T>
    auto componentStorage()
    {
        return std::get<PagedPool<T>>(mComponentStorage);
    }
};
}
