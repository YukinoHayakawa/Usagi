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
    const typename Traits::VertexIndexT child,
    Traits t)
    requires DirectedAcyclicGraph<G, Traits>
{
    // visit indirect descendents of v through child
    for(auto &&c : t.adjacent_vertices(g, child))
    {
        // c is reachable via v-child-c so remove the direct v-c edge if any
        t.remove_edge(g, v, c);

        transitive_reduce_helper<G, Traits>(g, v, c, t);
    }
}
}

// Q: Just flip the prev graph from longest_path_dag() might be faster?
// A: No. prev cannot deal with vertex whose outdegree > 1.
// e.g. 0->1, 2->1, 2->2 cannot be correctly reduced.
//
// todo: Any faster algorithm?
// Ref: https://cs.stackexchange.com/a/29133
template <
    typename G,
    typename Traits = typename DefaultGraphTrait<G>::TraitT
>
constexpr void transitive_reduce(G &g, Traits t = { })
    requires DirectedAcyclicGraph<G, Traits>
{
    // perform DFS on all vertices, assuming the graph is a DAG
    for(auto v = 0; v < t.num_vertices(g); ++v)
    {
        // visit children of v
        for(auto &&c : t.adjacent_vertices(g, v))
        {
            detail::transitive_reduce_helper<G, Traits>(g, v, c, t);
        }
    }
}
}
