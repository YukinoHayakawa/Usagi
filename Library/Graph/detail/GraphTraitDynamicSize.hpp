#pragma once

// #include <stack>
#include <vector>

#include "GraphTrait.hpp"

namespace usagi::graph
{
template <typename G>
struct GraphTraitDynamicSize
{
    template <typename T>
    using VertexAttributeArray = std::vector<T>;

    template <typename T>
    void resize(VertexAttributeArray<T> &array, std::size_t size) const
    {
        array.resize(size);
    }

    // using VertexIndexStack = std::stack<int>;
};
}
