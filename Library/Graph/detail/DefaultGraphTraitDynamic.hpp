#pragma once

#include <stack>
#include <vector>

namespace usagi::graph
{
template <typename G>
struct DefaultGraphTraitDynamic
{
    std::size_t size = 0;

    template <typename T>
    using VertexAttributeArray = std::vector<T>;

    template <typename T>
    void prepare(VertexAttributeArray<T> &array) const
    {
        array.resize(size);
    }

    using VertexIndexStack = std::stack<int>;

    explicit DefaultGraphTraitDynamic(const G &g)
    {
        size = g.num_vertices();
    }
};
}
