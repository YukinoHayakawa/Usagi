﻿#pragma once

#include <type_traits>

namespace usagi::ecs
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
 * 1. The task load is dispatched to a a different device than which issuing
 *    the tasks. Those devices may be a local device not sharing the main memory
 *    (such as a GPU) or another machine on the network.
 * 2. Part of the Entity Database is being mirrored to another machine (such as
 *    state syncing in multiplayer environments).
 * 3. Serialization.
 *
 * To avoid the complexity of reference management and resource management,
 * several constraints are imposed on the structure of a Component:
 *
 * 1. The Component must be memcpy-able.
 * 2. The Component must not have any reference or pointer types.
 * 3. The Component must not have any member variable that has reference or
 *    pointer types, or does any resource management.
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
 * References:
 *
 * https://docs.microsoft.com/en-us/cpp/cpp/trivial-standard-layout-and-pod-types
 */
template <typename T>
concept Component =
    // T has a determinative order of member variables and does not have
    // virtual stuff.
       std::is_standard_layout_v<T>
    && std::is_trivially_destructible_v<T>
    && std::is_nothrow_destructible_v<T>
;
}
