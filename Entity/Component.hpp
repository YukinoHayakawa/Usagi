﻿#pragma once

#include <concepts>

#include <Usagi/Concepts/Type/Memcpyable.hpp>

namespace usagi
{
/*
 * A Component is a set of semantically coherent variables. It is the C part
 * of the ECS architecture. An Entity may have multiple components of different
 * types which collaboratively define the characteristics of the Entity.
 *
 * The computation on Components are done by Systems.
 *
 * There are mainly three occasions where the content of a Component need to be
 * copied:
 *
 * -- The task load is dispatched to a a different device than which issuing
 *    the tasks. Those devices may be a local device not sharing the main memory
 *    (such as a GPU) or another machine on the network.
 * -- Part of the Entity Database is being mirrored to another machine (such as
 *    state syncing in multiplayer environments).
 * -- Serialization.
 *
 * To avoid the complexity of reference management and resource management,
 * several constraints are imposed on the structure of a Component:
 *
 * -- The Component must be memcpy-able and default constructible.
 * -- The Component must not have any reference or pointer types.
 * -- The Component must not have any member variable that has reference or
 *    pointer types, or does any resource management.
 * -- The Component and its member variables must not have any constant member
 *    variables.
 *
 * Ideally, these constraints should be check using metaprograms during compile
 * time. Due to the lack of reflection ability, it is by now not feasible.
 * Therefore, apart from some basic insurance on the layout of the Component,
 * the implementer of any Component instances must be aware of the intentions
 * of the design.
 *
 * This means that even if this concept evaluates to true for a given class,
 * it does not necessarily mean that it can be used as a Component.
 *
 * It's advisable to always use fixed width integer types to ensure the
 * portability across platforms.
 *
 * References:
 *
 * https://docs.microsoft.com/en-us/cpp/cpp/trivial-standard-layout-and-pod-types
 */
template <typename T>
concept SimpleComponentFilter = requires
{
    typename T::IsComponentFilter;
};

template <typename T>
concept Component =
    Memcpyable<T> &&
    // prevent accidentally using the component filter as a component
    !SimpleComponentFilter<T>
;

// todo: is it better to use sizeof(C) != 0 && std::is_empty_v<C>? (complete empty type)
template <typename C>
concept TagComponent = Component<C> && requires
{
    typename C::TagComponent;
};

template <typename Func, typename C>
concept ComponentProjection =
    std::invocable<Func, C &>;

template <typename Func, typename C>
concept WritableComponentProjection =
    ComponentProjection<Func, C> &&
    std::is_lvalue_reference_v<std::invoke_result_t<Func, C &>>
;

template <Component Comp>
struct GetComponent
{
    auto & operator()(auto &&entity_view)
    {
        return entity_view.template component<Comp>();
    }
};
}

#define USAGI_DECL_TAG_COMPONENT(name) \
    struct name { using TagComponent = void; } \
/**/
