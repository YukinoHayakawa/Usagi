#pragma once

#include <cassert>
#include <memory>

#include <Usagi/Entity/Component.hpp>
#include <Usagi/Library/Meta/Tag.hpp>

#include "ComponentAccess.hpp"
#include "ComponentFilter.hpp"
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
        // assert(mIndexInPage < page().first_unused_index);
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
        assert(include<C>());
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
            return BoolTag<C> { include<C>() };
        }
        else
        {
            // Locate the component in the storage
            auto idx = page().template component_page_index<C>();
            // Page not allocated
            if(idx == DatabaseT::EntityPageT::INVALID_PAGE)
                return nullptr;
            if(!include<C>())
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

    // Returns true only if this view actually points to some entity.
    bool valid() const
    {
        // Page is in valid storage
        const auto c0 = mPageIndex < this->entity_pages().size();
        if(!c0) return false;
        const auto &page = this->entity_pages().at(mPageIndex);
        // Page is actively allocated
        const auto c1 = page.page_seq_id != -1;
        if(!c1) return false;
        // Entity is allocated
        // const auto c2 = mIndexInPage < page.first_unused_index;
        // if(!c2) return false;
        return true;
        // todo: recognize destroyed entity or not?
    }

#ifdef __clang__
    __attribute__((used))
#endif
    PlainValueTupleT inspect() const
    {
        return inspection_helper(typename DatabaseT::ComponentFilterT());
    }

    template <Component C>
    bool include(C) const
    {
        return page().template component_bit<C>(mIndexInPage);
    }

    template <Component... C>
    bool include() const
    {
        // default to true when parameter pack C is empty
        return (... && include(C{}));
    }

    template <Component... C>
    bool include(ComponentFilter<C...>) const
    {
        return include<C...>();
    }

    template <Component C>
    bool exclude(C) const
    {
        return !include(C{});
    }

    template <Component... C>
    bool exclude() const
    {
        // default to true when parameter pack C is empty
        return (... && exclude(C{}));
    }

    template <Component... C>
    bool exclude(ComponentFilter<C...>) const
    {
        return exclude<C...>();
    }

    // Existing component value will be overwritten when calling this
    // function multiple times.
    template <Component C>
    decltype(auto) add_component(C val = { }) requires
        CanWriteComponent<ComponentAccess, C>
    {
        check_entity_created();

        // todo: perf - avoid setting dirty bit if the component already presents
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

            auto &ref = storage.at(idx)[mIndexInPage];
            return ref = val;
        }
    }

    template <Component C>
    void remove_component(C = {}) requires
        CanWriteComponent<ComponentAccess, C>
    {
        check_entity_created();

        // The entity should have the requested entity
        assert(include<C>());

        if constexpr(!TagComponent<C>)
        {
            [[maybe_unused]]
            // Locate the component position in the storage
            auto &idx = page().template component_page_index<C>();
            // auto &storage = this->template component_storage<C>();

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

    template <Component C>
    C & operator()(C) requires
        CanWriteComponent<ComponentAccess, C>
    {
        return component<C>();
    }

    template <Component C>
    const C & operator()(C) const requires
        (!CanWriteComponent<ComponentAccess, C>
        && CanReadComponent<ComponentAccess, C>)
    {
        return component<C>();
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
