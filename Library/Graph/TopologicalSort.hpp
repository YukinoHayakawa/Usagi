#pragma once

#include <Usagi/Concept/Type/Graph.hpp>
#include <Usagi/Library/Meta/Stack.hpp>

namespace usagi::graph
{
namespace detail
{
template <
    concepts::DirectedAcyclicGraph Graph,
    concepts::Stack<int> Stack = meta::Stack<Graph::Size>,
    typename Array = std::array<bool, Graph::Size>
>
constexpr void topological_sort_helper(
    const Graph &g,
    const int v,
    Stack &stack,
    Array &visited)
{
    visited[v] = true;

    // visit children of v
    for(auto &&c : g.adjacent_vertices(v))
    {
        if(!visited[c])
            topological_sort_helper(g, c, stack, visited);
    }

    // ensure the current vertex precedes the children when popping
    // from the stack
    stack.push(v);
}
}

// Ref: https://www.geeksforgeeks.org/topological-sorting/
template <
    concepts::DirectedAcyclicGraph Graph,
    concepts::Stack<int> Stack = meta::Stack<Graph::SIZE>,
    typename Array = std::array<bool, Graph::SIZE>
>
constexpr auto topological_sort(const Graph &g)
{
    Stack stack;
    Array visited { };

    for(auto v = 0; v < g.num_vertices(); ++v)
        if(!visited[v])
            detail::topological_sort_helper(g, v, stack, visited);

    return stack;
}
}
