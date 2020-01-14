#pragma once

namespace usagi
{
// Systems
//
// - Basics
//
// The System in the ECS architecture operates on the Entities by requesting
// views from the Entity Database. Ideally, the Systems should be stateless,
// meaning that they manipulates the Entities regardless of the states of the
// entities in the previous frames. It should also not explicitly hold any
// resources such as memory and textures. Those resources should be centrally
// managed by Runtime Services.
//
// - Resources Handling
//
// These two requirements imply that a System should be able to be added and
// removed during runtime outside the execution period of the System itself
// without causing Game faults or resource leaks. When a System intends to
// refer to a Resource such as a Texture, it should use pass a handle or
// identifier, depending on the exact type of the resource, and pass it to the
// corresponding Runtime Subsystem. For example, when a Texture shall be
// referenced, the System asks the Runtime Services for a handle of the Texture
// by providing its Name. The Runtime Services may report that the Asset is not
// present in the memory and have to be loaded from other sources. The System
// can decide upon that status and perform a proper action, such as using a
// fallback asset instead, or skip the rendering action. The System may also
// declare whether the requested is critical for the Gameplay or not. The
// Runtime Services can use that declarations to decide whether to evict
// unimportant Assets from the memory when the memory is scarce.
//
// TODO how to manage assets created during runtime & dynamic buffers
//
// - Example
//
// A System should declare its Read and Write Access Masks to Components. These
// masks are used by the Executive to manage the Execution dependency and order
// of all the Systems. (This might be changed in the future if there is any way
// of inferring the Component accesses from the System implementations.) The
// only member function must be supported is the `update()` template, which
// takes a RuntimeServiceProvider and a EntityDatabaseAccess. The exact types
// of those two arguments depends on the actual Database, since the Systems
// might be applied to different setups of Entity Databases. Ideally, the
// Executive should be able to infer a desirable Database type with all used
// Component types from the Systems registered by the Game Script.
//
// Below is an example System:
//
// struct SystemFireworksUpdate
// {
//     using ReadAccess = ComponentFilter<ComponentFireworks>;
//
//     template <typename RuntimeServiceProvider, typename EntityDatabaseAccess>
//     void update(RuntimeServiceProvider &&rt, EntityDatabaseAccess &&db)
//     {
//         std::size_t i = 0;
//         for(auto &&e : db.view(ReadAccess()))
//         {
//             component<ComponentFireworks>(e).time_to_explode = (float)++i;
//         }
//     }
// };
template <typename T>
concept System = requires(T t)
{
    // { T::ReadAccess };
    // { T::WriteAccess };
};

template <typename T>
concept SystemHasReadAccessMask =
    System<T> && requires (T) { T::ReadAccess; };

template <typename T>
concept SystemHasWriteAccessMask =
    System<T> && requires (T) { T::WriteAccess; };
}
