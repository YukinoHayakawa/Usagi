#pragma once

#include <Usagi/Library/Meta/Stack.hpp>

#include "GraphTrait.hpp"

namespace usagi::graph
{
template <typename G, std::size_t Size>
struct GraphTraitFixedSize
{
    template <typename T>
    using VertexAttributeArray = std::array<T, Size>;

    template <typename T>
    static constexpr void resize(VertexAttributeArray<T> &, std::size_t)
    {
        /* no-op */
    }

    using VertexIndexStack = meta::Stack<Size>;
};
}
