#pragma once

#include <cassert>

#include <Usagi/Experimental/v2/Game/Entity/Component.hpp>

#include "EntityDatabaseInternalAccess.hpp"
#include "ComponentAccess.hpp"
#include "ComponentFilter.hpp"

namespace usagi
{
template <
    typename Database,
    typename ComponentAccess
>
class EntityView
    : EntityDatabaseInternalAccess<Database>
{
    using DatabaseT         = Database;
    using EntityPageT       = typename DatabaseT::EntityPageT;
    using ComponentMaskT    = typename DatabaseT::ComponentMaskT;

    EntityPageT     *mPage;
    std::size_t     mIndexInPage;

    template <Component C>
    C & componentAccess()
    {
        // Locate the component in the storage
        auto &idx = mPage->template componentPageIndex<C>();
        auto &storage = this->mDatabase->template componentStorage<C>();
        // Ensure that the entity has the component
        assert(hasComponents<C>());
        assert(idx != DatabaseT::EntityPageT::INVALID_PAGE);
        // Access the component in the storage page
        return storage.block(idx)[mIndexInPage];
    }

    auto & entityStateRef() const
    {
        return mPage->entity_states[mIndexInPage];
    }

    template <Component... C>
    static constexpr auto buildComponentMask()
    {
        constexpr auto mask = DatabaseT::template buildComponentMask<C...>();
        // static_assert(DatabaseT::template buildComponentMask<C...>() != 0);
        return mask;
    }

public:
    EntityView(
        DatabaseT *database,
        EntityPageT *page,
        const std::size_t index_in_page)
        : EntityDatabaseInternalAccess<Database>(database)
        , mPage(page)
        , mIndexInPage(index_in_page)
    {
    }

    template <Component... C>
    bool hasComponents() const
    {
        constexpr auto mask = buildComponentMask<C...>();
        assert(mask.any());
        return (entityStateRef().owned & mask) == mask;
    }

    template <Component... C>
    bool checkIncludeFilter(ComponentFilter<C...> filter) const
    {
        return hasComponents<C...>();
    }

    template <Component... C>
    bool checkExcludeFilter(ComponentFilter<C...> filter) const
    {
        constexpr auto mask = buildComponentMask<C...>();
        return (entityStateRef().owned & mask).none();
    }

    auto componentMask() const
    {
        return entityStateRef().owned;
    }

    template <Component C>
    C & addComponent() requires HasComponentWriteAccess<ComponentAccess, C>
    {
        // Locate the component position in the storage
        auto &idx = mPage->template componentPageIndex<C>();
        auto &storage = this->mDatabase->template componentStorage<C>();
        // The entity shouldn't have the requested entity
        assert(!hasComponents<C>());
        // If the component page hasn't be allocated yet, allocate it.
        if(idx == EntityPageT::INVALID_PAGE)
            idx = storage.allocate();
        // Turn on the owned component bit
        entityStateRef().owned |=
            this->mDatabase->template componentMaskBit<C>();
        // todo Mark entity page dirty - calc component mask in iterator dtor?
        // mDatabase->markEntityDirty(mId);
        return storage.block(idx)[mIndexInPage];
    }

    template <Component C>
    void removeComponent() requires HasComponentWriteAccess<ComponentAccess, C>
    {
        // Locate the component position in the storage
        auto &idx = mPage->template componentPageIndex<C>();
        auto &storage = this->mDatabase->template componentStorage<C>();
        // The entity should have the requested entity
        assert(hasComponents<C>());
        assert(idx != EntityPageT::INVALID_PAGE);
        // Turn off the owned component bit
        entityStateRef().owned &=
            ~this->mDatabase->template componentMaskBit<C>();
        // todo Mark entity page dirty
        // mDatabase->markEntityDirty(mId);
        // TODO if all component of the same kind in that page were removed,
        // the component page can be deallocated. -> in page iterator?
    }

    // todo: compile time check of component type (type must be included in the db etc)
    /**
     * \brief Read & write access to the component.
     * \tparam C
     * \return
     */
    template <Component C>
    C & component() requires
        HasComponentWriteAccess<ComponentAccess, C>
    {
        return componentAccess<C>();
    }

    /**
     * \brief Read-only access to the component.
     * \tparam C
     * \return
     */
    template <Component C>
    const C & component() const requires
        !HasComponentReadAccess<ComponentAccess, C>
        && HasComponentReadAccess<ComponentAccess, C>
    {
        return componentAccess<C>();
    }
};

template <Component C, typename EntityView>
decltype(auto) component(EntityView &&view)
{
    return std::forward<EntityView>(view).template component<C>();
}
}

/**
 * \brief This macro performs static_cast on the returned type of Entity View
 * thus turning the dependent name into a non-dependent one for the sake
 * of code auto-completion.
 * \param entity_view
 * \param component_type
 */
#define USAGI_COMPONENT(entity_view, component_type) \
    static_cast<::usagi::detail::ComponentReferenceType< \
        std::remove_reference_t<decltype(*this)>, \
        component_type \
    >>(::usagi::component<component_type>(entity_view)) \
/**/
