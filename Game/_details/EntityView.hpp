#pragma once

#include <Usagi/Experimental/v2/Game/Entity/Component.hpp>

namespace usagi
{
template <
    typename Database,
    typename PermissionValidator
>
class EntityView
{
    using DatabaseT         = Database;
    using EntityPageT       = typename DatabaseT::EntityPageT;
    using ComponentMaskT    = typename DatabaseT::ComponentMaskT;

    DatabaseT       *mDatabase;
    EntityPageT     *mPage;
    std::size_t     mIndexInPage;

    template <Component C>
    C & componentAccess()
    {
        // Locate the component in the storage
        auto &idx = mPage->template componentPageIndex<C>();
        auto &storage = mDatabase->template componentStorage<C>();
        // Ensure that the entity has the component
        assert(hasComponents<C>());
        assert(idx != DatabaseT::EntityPageT::INVALID_PAGE);
        // Access the component in the storage page
        return storage->page(idx)[mIndexInPage];
    }

public:
    EntityView(
        DatabaseT *database,
        EntityPageT *page,
        const std::size_t index_in_page)
        : mDatabase(database)
        , mPage(page)
        , mIndexInPage(index_in_page)
    {
    }

    template <Component... C>
    bool hasComponents() const
    {
        ComponentMaskT mask;
        ((mask |= mDatabase->template componentMaskBit<C>()), ...);
        return (mPage->entity_states[mIndexInPage].owned & mask) == mask;
    }

    auto componentMask() const
    {
        return mPage->entity_states[mIndexInPage].owned;
    }

    template <Component C>
    C & addComponent()
    {
        static_assert(
            PermissionValidator::template hasWriteAccess<C>(),
            "No write access to the component."
        );

        // Locate the component position in the storage
        auto &idx = mPage->template componentPageIndex<C>();
        auto &storage = mDatabase->template componentStorage<C>();
        // The entity shouldn't have the requested entity
        assert(!hasComponents<C>());
        // If the component page hasn't be allocated yet, allocate it.
        if(idx == EntityPageT::INVALID_PAGE)
            idx = storage.allocate();
        // Turn on the owned component bit
        mPage->entity_states[mIndexInPage].owned |=
            mDatabase->template componentMaskBit<C>();
        // todo Mark entity page dirty - calc component mask in iterator dtor?
        // mDatabase->markEntityDirty(mId);
        return storage.block(idx)[mIndexInPage];
    }

    template <Component C>
    void removeComponent()
    {
        static_assert(
            PermissionValidator::template hasWriteAccess<C>(),
            "No write access to the component."
        );

        // Locate the component position in the storage
        auto &idx = mPage->template componentPageIndex<C>();
        auto &storage = mDatabase->template componentStorage<C>();
        // The entity should have the requested entity
        assert(hasComponents<C>());
        assert(idx != EntityPageT::INVALID_PAGE);
        // Turn off the owned component bit
        mPage->entity_states[mIndexInPage].owned &=
            ~mDatabase->template componentMaskBit<C>();
        // todo Mark entity page dirty
        // mDatabase->markEntityDirty(mId);
        // TODO if all component of the same kind in that page were removed,
        // the component page can be deallocated. -> in page iterator?
    }

    /**
     * \brief Read & write access to the component.
     * \tparam C
     * \return
     */
    template <Component C>
    std::enable_if_t<PermissionValidator::template hasWriteAccess<C>(), C &>
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
    std::enable_if_t<PermissionValidator::template hasReadAccess<C>() &&
        !PermissionValidator::template hasWriteAccess<C>(), const C &>
        operator()() const
    {
        return componentAccess<C>();
    }
};
}
