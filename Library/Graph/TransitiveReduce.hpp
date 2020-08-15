#pragma once

#include <Usagi/Concept/Type/Graph.hpp>

namespace usagi::graph
{
namespace detail
{
template <concepts::DirectedAcyclicGraph Graph>
constexpr void transitive_reduce_helper(
    Graph &g,
    const int v,
    const int child)
{
    // visit indirect descendents of v through child
    for(auto &&c : g.adjacent_vertices(child))
    {
        // otherwise, there is an indirect path from v to i, remove the
        // direct path from v to i.
        g.remove_edge(v, c);

        transitive_reduce_helper(g, v, c);
    }
}
}

// Ref: https://cs.stackexchange.com/a/29133
template <concepts::DirectedAcyclicGraph Graph>
constexpr void transitive_reduce(Graph &g)
{
    // perform DFS on all vertices, assuming the graph is a DAG
    for(auto v = 0; v < g.num_vertices(); ++v)
    {
        // visit children of v
        for(auto &&c : g.adjacent_vertices(v))
        {
            detail::transitive_reduce_helper(g, v, c);
        }
    }
}
}
