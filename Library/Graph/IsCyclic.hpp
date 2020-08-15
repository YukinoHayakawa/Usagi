#pragma once

#include <array>

#include <Usagi/Concept/Type/Graph.hpp>

namespace usagi::graph
{
namespace detail
{
template <
    concepts::DirectedGraph Graph,
    typename BitArray = std::array<bool, Graph::SIZE>
>
constexpr bool is_cyclic_helper(
    const Graph &g,
    int v,
    BitArray &visited,
    BitArray &visiting)
{
    if(!visited[v])
    {
        visited[v] = true;
        // if this vertex is not visited, it cannot be on the recursive
        // stack
        visiting[v] = true;

        // visit children of v
        for(auto &&c : g.adjacent_vertices(v))
        {
            // not connected
            if(g.has_edge(v, c) == false)
                continue;
            // new child, dive in
            if(!visited[c])
            {
                if(is_cyclic_helper(g, c, visited, visiting))
                    return true;
            }
            // getting back to one of the ancestor -> a cycle was found
            else if(visiting[c])
                return true;
        }

        visiting[v] = false;
    }
    return false;
}
}

template <
    concepts::DirectedGraph Graph,
    typename BitArray = std::array<bool, Graph::SIZE>
>
// Ref: https://www.geeksforgeeks.org/detect-cycle-in-a-graph/
constexpr bool is_cyclic(const Graph &g)
{
    BitArray visited { };
    BitArray visiting { };

    for(auto i = 0; i < g.num_vertices(); ++i)
    {
        if(detail::is_cyclic_helper(g, i, visited, visiting))
            return true;
    }
    return false;
}
}
