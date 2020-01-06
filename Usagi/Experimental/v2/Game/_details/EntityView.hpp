#pragma once

#include <type_traits>

#include "EntityId.hpp"

namespace usagi::ecs
{
template <
    typename Database,
    typename PermissionChecker
>
class EntityView
{
    using DatabaseT = Database;
    using EntityPageT = typename DatabaseT::EntityPageT;
    using ComponentMaskT = typename DatabaseT::ComponentMask;

    DatabaseT &mDatabase;
    EntityId mId;
    DatabaseT::EntityPageT &mPage = mDatabase.entityPage(mId);

    template <Component C>
    std::size_t indexInPage() const
    {
        return mId.id % DatabaseT::ENTITY_PAGE_SIZE;
    }

    template <Component C>
    C & componentAccess()
    {
        // Locate the component in the storage
        auto &idx = mPage->componentPageIndex<C>();
        auto &storage = mDatabase->componentStorage<C>();
        const pidx = indexInPage();
        // Ensure that the entity has the component
        assert(hasComponents<C>());
        assert(idx != DatabaseT::EntityPageT::INVALID_PAGE);
        // Access the component in the storage page
        return storage->page(idx)[pidx];
    }

public:
    EntityView(Database &database, EntityId id)
        : mDatabase(database)
        , mId(std::move(id))
    {
    }

    template <Component... C>
    bool hasComponents() const
    {
        ComponentMaskT mask;
        (mask |= mDatabase.componentMaskBit<C>(), ...);
        return mPage.entity_states[indexInPage()].owned & mask == mask;
    }

    template <Component C>
    std::enable_if<PermissionChecker::hasWriteAccess<C>(), C &>
        addComponent()
    {
        // Locate the component position in the storage
        auto &idx = mPage->componentPageIndex<C>();
        auto &storage = mDatabase->componentStorage<C>();
        const pidx = indexInPage();
        // The entity shouldn't have the requested entity
        assert(!hasComponents<C>());
        // If the component page hasn't be allocated yet, allocate it.
        if(idx == EntityPageT::INVALID_PAGE)
            idx = storage.allocate();
        // Turn on the owned component bit
        mPage.entity_states[pidx].owned |= mDatabase.componentMaskBit<C>();
        // Mark entity page dirty
        mDatabase.markEntityDirty(mId);
        return storage->page(idx)[pidx];
    }

    template <Component C>
    std::enable_if<!PermissionChecker::hasWriteAccess<C>(), C &>
        addComponent()
    {
        static_assert(false, "No write access to the component.");
    }

    template <Component C>
    std::enable_if<PermissionChecker::hasWriteAccess<C>()>
        removeComponent()
    {
        // Locate the component position in the storage
        auto &idx = mPage->componentPageIndex<C>();
        auto &storage = mDatabase->componentStorage<C>();
        const pidx = indexInPage();
        // The entity should have the requested entity
        assert(hasComponents<C>());
        assert(idx != EntityPageT::INVALID_PAGE);
        // Turn off the owned component bit
        mPage.entity_states[pidx].owned &= ~mDatabase.componentMaskBit<C>();
        // Mark entity page dirty
        mDatabase.markEntityDirty(mId);
        // TODO if all component of the same kind in that page were removed,
        // the component page can be deallocated. -> in page iterator?
    }

    template <Component C>
    std::enable_if<!PermissionChecker::hasWriteAccess<C>()>
        removeComponent()
    {
        static_assert(false, "No write access to the component.");
    }

    /**
     * \brief Read & write access to the componenet.
     * \tparam C
     * \return
     */
    template <Component C>
    std::enable_if<PermissionChecker::hasWriteAccess<C>(), const C &>
        operator()()
    {
        return componentAccess<C>();
    }

    /**
     * \brief Read-only access to the component.
     * \tparam C
     * \return
     */
    template <Component C>
    std::enable_if<PermissionChecker::hasReadAccess<C>() &&
        !PermissionChecker::hasWriteAccess<C>(), C &>
        operator()()
    {
        return componentAccess<C>();
    }

    template <Component C>
    std::enable_if<!PermissionChecker::hasReadAccess<C>()>
        operator()()
    {
        static_assert(false, "No read access to the component.");
    }
};
}
