#pragma once

#include "Graph.hpp"

namespace usagi::graph
{
namespace detail
{
template <
    typename G,
    typename Traits = typename DefaultGraphTrait<G>::TraitT
>
constexpr void transitive_reduce_helper(
    G &g,
    const typename Traits::VertexIndexT v,
    const typename Traits::VertexIndexT child)
    requires DirectedAcyclicGraph<G, Traits>
{
    Traits t;

    // visit indirect descendents of v through child
    for(auto &&c : t.adjacent_vertices(g, child))
    {
        // otherwise, there is an indirect path from v to i, remove the
        // direct path from v to i.
        t.remove_edge(g, v, c);

        transitive_reduce_helper(g, v, c);
    }
}
}

// Ref: https://cs.stackexchange.com/a/29133
template <
    typename G,
    typename Traits = typename DefaultGraphTrait<G>::TraitT
>
constexpr void transitive_reduce(G &g)
    requires DirectedAcyclicGraph<G, Traits>
{
    Traits t;

    // perform DFS on all vertices, assuming the graph is a DAG
    for(auto v = 0; v < t.num_vertices(g); ++v)
    {
        // visit children of v
        for(auto &&c : t.adjacent_vertices(g, v))
        {
            detail::transitive_reduce_helper(g, v, c);
        }
    }
}
}
