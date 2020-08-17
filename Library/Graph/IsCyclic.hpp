#pragma once

#include <array>

#include <Usagi/Concept/Type/Graph.hpp>

namespace usagi::graph
{
namespace detail
{
template <
    concepts::DirectedGraph Graph,
    typename Traits
>
constexpr bool is_cyclic_helper(
    const Graph &g,
    int v,
    typename Traits::template VertexAttributeArray<bool> &visited,
    typename Traits::template VertexAttributeArray<bool> &visiting)
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
                if(is_cyclic_helper<Graph, Traits>(g, c, visited, visiting))
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

/**
 * \brief Check whether the given directed graph has any cycle.
 *
 * Ref: https://www.geeksforgeeks.org/detect-cycle-in-a-graph/
 * \tparam Graph
 * \tparam Traits
 * \param g
 * \return
 */
template <
    concepts::DirectedGraph Graph,
    typename Traits = typename Graph::trait_t
>
constexpr bool is_cyclic(const Graph &g)
{
    auto traits = Traits(g);

    typename Traits::template VertexAttributeArray<bool> visited { };
    typename Traits::template VertexAttributeArray<bool> visiting { };
    traits.prepare(visited);
    traits.prepare(visiting);

    for(auto i = 0; i < g.num_vertices(); ++i)
    {
        if(detail::is_cyclic_helper<Graph, Traits>(g, i, visited, visiting))
            return true;
    }
    return false;
}
}
