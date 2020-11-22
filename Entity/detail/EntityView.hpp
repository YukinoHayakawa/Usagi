#pragma once

#include <cassert>
#include <memory>

#include <Usagi/Entity/Component.hpp>
#include <Usagi/Library/Meta/Tag.hpp>

#include "ComponentAccess.hpp"
#include "ComponentFilter.hpp"
#include "ComponentTrait.hpp"
#include "EntityDatabaseAccessInternal.hpp"
#include "EntityId.hpp"

namespace usagi
{
template <
    typename Database,
    typename ComponentAccess = ComponentAccessReadOnly
>
class EntityView
    : EntityDatabaseAccessInternal<Database>
{
    template <typename OtherDatabase, typename OtherAccess>
    friend class EntityView;

    using DatabaseT         = Database;
    using EntityPageT       = typename DatabaseT::EntityPageT;
    using EntityIndexT      = typename EntityPageT::EntityIndexT;

    std::size_t     mPageIndex = EntityPageT::INVALID_PAGE;
    EntityIndexT    mIndexInPage = -1;

    void check_entity_created() const
    {
        assert(mIndexInPage < page().first_unused_index);
    }

    auto & page() const
    {
        return this->entity_page_at_index(mPageIndex).ref();
    }

    template <Component C>
    C & component_access() const
        requires (!TagComponent<C>)
    {
        check_entity_created();

        // Locate the component in the storage
        auto idx = page().template component_page_index<C>();
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
        (..., page().template reset_component_bit<C>(mIndexInPage));
    }

    /*
     * Debugging Helpers
     */

    template <Component C>
    using PlainValueT = std::conditional_t<
        TagComponent<C>, BoolTag<C>, const C *
    >;

    template <Component... Cs>
    static auto inspection_tuple_type(ComponentFilter<Cs...>)
    {
        return std::tuple<PlainValueT<Cs>...>();
    }

    using PlainValueTupleT = decltype(
        inspection_tuple_type(typename DatabaseT::ComponentFilterT())
    );

    template <Component C>
    PlainValueT<C> component_value_unchecked() const
    {
        if constexpr(TagComponent<C>)
        {
            return BoolTag<C> { has_component<C>() };
        }
        else
        {
            // Locate the component in the storage
            auto idx = page().template component_page_index<C>();
            // Page not allocated
            if(idx == DatabaseT::EntityPageT::INVALID_PAGE)
                return nullptr;
            if(!has_component<C>())
                return nullptr;
            auto &storage = this->template component_storage<C>();
            // Access the component in the storage page
            return &storage.at(idx)[mIndexInPage];
        }
    }

    template <Component... Cs>
    auto inspection_helper(ComponentFilter<Cs...>) const
    {
        return std::make_tuple(component_value_unchecked<Cs>()...);
    }

public:
    EntityView() = default;

    EntityView(
        DatabaseT *database,
        const std::size_t page_index,
        const EntityIndexT index_in_page)
        : EntityDatabaseAccessInternal<Database>(database)
        , mPageIndex(page_index)
        , mIndexInPage(index_in_page)
    {
    }

    template <typename OtherAccess>
    EntityView(const EntityView<DatabaseT, OtherAccess> &other)
        requires std::is_same_v<ComponentAccess, ComponentAccessReadOnly>
        : EntityDatabaseAccessInternal<Database>(other.mDatabase)
        , mPageIndex(other.mPageIndex)
        , mIndexInPage(other.mIndexInPage)
    {

    }

    EntityId id() const
    {
        return {
            .offset = mIndexInPage,
            .page = mPageIndex
        };
    }

#ifdef __clang__
    __attribute__((used))
#endif
    PlainValueTupleT inspect() const
    {
        return inspection_helper(typename DatabaseT::ComponentFilterT());
    }

    template <Component C>
    bool has_component(C = {}) const
    {
        return page().template component_bit<C>(mIndexInPage);
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
        check_entity_created();

        if constexpr(TagComponent<C>)
        {
            page().template set_component_bit<C>(mIndexInPage);
            page().dirty = true;
        }
        else
        {
            // Locate the component position in the storage
            auto &idx = page().template component_page_index<C>();
            auto &storage = this->template component_storage<C>();

            // If the component page hasn't be allocated yet, allocate it.
            if(idx == EntityPageT::INVALID_PAGE)
            {
                idx = storage.allocate();
                std::allocator_traits<std::remove_reference_t<
                    decltype(storage)
                >>::construct(storage, &storage.at(idx));
            }

            page().template set_component_bit<C>(mIndexInPage);
            page().dirty = true;

            return storage.at(idx)[mIndexInPage] = val;
        }
    }

    template <Component C>
    void remove_component(C = {}) requires
        CanWriteComponent<ComponentAccess, C>
    {
        check_entity_created();

        if constexpr(!TagComponent<C>)
        {
            // Locate the component position in the storage
            auto &idx = page().template component_page_index<C>();
            auto &storage = this->template component_storage<C>();

            // The entity should have the requested entity
            assert(has_component<C>());
            assert(idx != EntityPageT::INVALID_PAGE);
        }

        page().template reset_component_bit<C>(mIndexInPage);
        page().dirty = true;
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
        check_entity_created();

        reset_component_bits(typename DatabaseT::ComponentFilterT());
        page().dirty = true;
    }

    // todo: compile time check of component type (type must be included in the db etc)
    /**
     * \brief Read & write access to the component.
     * \tparam C
     * \return
     */
    template <Component C>
    C & component(C = {}) requires
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
    const C & component(C = {}) const requires
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
