#pragma once

#include <cassert>
#include <memory>

#include <Usagi/Entity/Component.hpp>
#include <Usagi/Library/Meta/Tag.hpp>

#include "EntityDatabaseAccessInternal.hpp"
#include "ComponentAccess.hpp"
#include "ComponentFilter.hpp"
#include "ComponentTrait.hpp"

namespace usagi
{
template <
    typename Database,
    typename ComponentAccess
>
class EntityView
    : EntityDatabaseAccessInternal<Database>
{
    using DatabaseT         = Database;
    using EntityPageT       = typename DatabaseT::EntityPageT;
    using EntityIndexT      = typename EntityPageT::EntityIndexT;

    EntityPageT     *mPage;
    EntityIndexT    mIndexInPage;

    template <Component C>
    C & component_access() const
        requires (!IsFlagComponent<C>)
    {
        // Locate the component in the storage
        auto &idx = mPage->template component_page_index<C>();
        auto &storage = this->template component_storage<C>();
        // Ensure that the entity has the component
        assert(has_component<C>());
        assert(idx != DatabaseT::EntityPageT::INVALID_PAGE);
        // Access the component in the storage page
        return storage.at(idx)[mIndexInPage];
    }

    template <Component... C>
    void reset_component_bits(ComponentFilter<C...>)
    {
        (..., mPage->template reset_component_bit<C>(mIndexInPage));
    }

public:
    EntityView(
        DatabaseT *database,
        EntityPageT *page,
        const EntityIndexT index_in_page)
        : EntityDatabaseAccessInternal<Database>(database)
        , mPage(page)
        , mIndexInPage(index_in_page)
    {
    }

    template <Component C>
    bool has_component() const
    {
        return mPage->template component_bit<C>(mIndexInPage);
    }

    template <Component... C>
    bool has_components() const
    {
        return (... || has_component<C>());
    }

    template <Component C>
    auto add_component(C val = { }) requires
        CanWriteComponent<ComponentAccess, C>
    {
        // The entity shouldn't have the requested entity
        assert(!has_component<C>());

        if constexpr(IsFlagComponent<C>)
        {
            mPage->template set_component_bit<C>(mIndexInPage);
            mPage->dirty = true;
        }
        else
        {
            // Locate the component position in the storage
            auto &idx = mPage->template component_page_index<C>();
            auto &storage = this->template component_storage<C>();

            // If the component page hasn't be allocated yet, allocate it.
            if(idx == EntityPageT::INVALID_PAGE)
            {
                idx = storage.allocate();
                std::allocator_traits<std::remove_reference_t<
                    decltype(storage)
                >>::construct(storage, &storage.at(idx));
            }

            mPage->template set_component_bit<C>(mIndexInPage);
            mPage->dirty = true;

            return storage.at(idx)[mIndexInPage] = val;
        }
    }

    template <Component C>
    decltype(auto) add_component(Tag<C>)
    {
        return add_component<C>();
    }

    template <Component C>
    void remove_component() requires
        CanWriteComponent<ComponentAccess, C>
    {
        if constexpr(!IsFlagComponent<C>)
        {
            // Locate the component position in the storage
            auto &idx = mPage->template component_page_index<C>();
            auto &storage = this->template component_storage<C>();

            // The entity should have the requested entity
            assert(has_component<C>());
            assert(idx != EntityPageT::INVALID_PAGE);
        }

        mPage->template reset_component_bit<C>(mIndexInPage);
        mPage->dirty = true;
    }

    template <Component C>
    void remove_component(Tag<C>) requires
        CanWriteComponent<ComponentAccess, C>
    {
        remove_component<C>();
    }

    // requires write permissions to all components in the database.
    // alternatively, only remove relevant components in an entity.
    // if an entity does not have any component, it is considered removed.
    void destroy() requires
        CanWriteComponentsByFilter<
            ComponentAccess,
            typename DatabaseT::ComponentFilterT
        >
    {
        reset_component_bits(typename DatabaseT::ComponentFilterT());
        mPage->dirty = true;
    }

    // todo: compile time check of component type (type must be included in the db etc)
    /**
     * \brief Read & write access to the component.
     * \tparam C
     * \return
     */
    template <Component C>
    C & component() requires
        CanWriteComponent<ComponentAccess, C>
    {
        return component_access<C>();
    }

    /**
     * \brief Read-only access to the component.
     * \tparam C
     * \return
     */
    template <Component C>
    const C & component() const requires
        (!CanWriteComponent<ComponentAccess, C>
        && CanReadComponent<ComponentAccess, C>)
    {
        return component_access<C>();
    }
};

template <Component C, typename EntityView>
decltype(auto) component(EntityView &&view)
{
    return std::forward<EntityView>(view).template component<C>();
}
}

// ComponentReferenceType
#include "ComponentAccessSystemAttribute.hpp"

/**
 * \brief This macro performs static_cast on the returned type of Entity View
 * thus turning the dependent name into a non-dependent one for the sake
 * of code auto-completion.
 * \param entity_view
 * \param component_type
 */
#define USAGI_COMPONENT(entity_view, component_type) \
    static_cast<::usagi::ComponentReferenceType< \
        std::remove_pointer_t<decltype(this)>, \
        component_type \
    >>(::usagi::component<component_type>(entity_view)) \
/**/
