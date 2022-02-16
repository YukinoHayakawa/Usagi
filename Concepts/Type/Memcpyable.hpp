#pragma once

#include <type_traits>

namespace usagi
{
// Ref: https://docs.microsoft.com/en-us/cpp/cpp/trivial-standard-layout-and-pod-types
template <typename T>
concept Memcpyable =
    // T has a determinative order of member variables and does not have
    // virtual stuff.
       std::is_standard_layout_v<T>
    // T does not manage any resources. The standard indicates:
    // Storage occupied by trivially destructible objects may be reused without
    // calling the destructor.
    // This is important for efficient pooling of components.
    && std::is_trivially_destructible_v<T>
    && std::is_default_constructible_v<T>
    // Basically all destructors satisfy this requirement. Just in case.
    && std::is_nothrow_destructible_v<T>
;
}
