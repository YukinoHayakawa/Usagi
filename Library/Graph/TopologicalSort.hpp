#pragma once

#include <Usagi/Library/Meta/Stack.hpp>

#include "GraphAdjacencyMatrix.hpp"

namespace usagi
{
namespace detail
{
template <int Size>
constexpr void topological_sort_helper(
    const GraphAdjacencyMatrix<Size> &g,
    const int v,
    meta::Stack<Size> &stack,
    std::array<bool, Size> &visited)
{
    visited[v] = true;

    // visit children of v
    for(auto i = 0; i < Size; ++i)
    {
        // not child of v
        if(g.matrix[v][i] == false)
            continue;

        if(!visited[i])
            topological_sort_helper<Size>(g, i, stack, visited);
    }

    // ensure the current vertex precedes the children when popping
    // from the stack
    stack.push(v);
}
}

// Ref: https://www.geeksforgeeks.org/topological-sorting/
template <int Size>
constexpr auto topological_sort(const GraphAdjacencyMatrix<Size> &g)
{
    meta::Stack<Size> stack;
    std::array<bool, Size> visited { };

    for(auto v = 0; v < Size; ++v)
    {
        if(!visited[v])
            detail::topological_sort_helper<Size>(g, v, stack, visited);
    }

    return stack;
}
}
