#pragma once

#include <array>

#include "Graph.hpp"

namespace usagi::graph
{
namespace detail
{
template <
    typename G,
    typename Traits = typename DefaultGraphTrait<G>::TraitT
>
constexpr bool is_cyclic_helper(
    const G &g,
    const typename Traits::VertexIndexT v,
    typename Traits::template VertexAttributeArray<bool> &visited,
    typename Traits::template VertexAttributeArray<bool> &visiting,
    Traits t)
    requires DirectedGraph<G, Traits>
{
    if(!visited[v])
    {
        visited[v] = true;
        // if this vertex is not visited, it cannot be on the recursive
        // stack
        visiting[v] = true;

        // visit children of v
        for(auto &&c : t.adjacent_vertices(g, v))
        {
            // not connected
            if(t.has_edge(g, v, c) == false)
                continue;
            // new child, dive in
            if(!visited[c])
            {
                if(is_cyclic_helper<G, Traits>(g, c, visited, visiting, t))
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
    typename G,
    typename Traits = typename DefaultGraphTrait<G>::TraitT
>
constexpr bool is_cyclic(const G &g, Traits t = { })
    requires DirectedGraph<G, Traits>
{
    typename Traits::template VertexAttributeArray<bool> visited { };
    typename Traits::template VertexAttributeArray<bool> visiting { };
    t.resize(visited, t.num_vertices(g));
    t.resize(visiting, t.num_vertices(g));

    for(auto i = 0; i < t.num_vertices(g); ++i)
    {
        if(detail::is_cyclic_helper<G, Traits>(g, i, visited, visiting, t))
            return true;
    }
    return false;
}
}
