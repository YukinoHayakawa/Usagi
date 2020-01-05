#pragma once

#include <type_traits>

namespace usagi::ecs
{
template <typename T>
concept Pageable =
    std::is_default_constructible_v<T> &&
    std::is_trivially_destructible_v<T> &&
    std::is_move_constructible_v<T> &&
    // memcpy-able
    std::is_standard_layout_v<T>
;

/*
* A component is a set of semantically coherent variables, without any
* direct references to memory and dynamic allocation so it is trvially
* copyable between devices.
*
* Alignment:
* https://stackoverflow.com/questions/45478824/c-alignment-when-to-use-alignas/49717229
*
* Layout:
* https://docs.microsoft.com/en-us/cpp/cpp/trivial-standard-layout-and-pod-types?view=vs-2019
*/
template <typename T>
concept Component =
    // aligned to cache line
    // probably deal with alignment in allocator
    // (std::alignment_of_v<T> == 64) &&
    Pageable<T>
;


template <typename T>
concept System = requires (T sys) {

};
}
