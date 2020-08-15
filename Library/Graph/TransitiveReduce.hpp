#pragma once

#include <Usagi/Concept/Type/Graph.hpp>

namespace usagi::graph
{
namespace detail
{
template <concepts::DirectedGraph Graph>
constexpr void transitive_reduce_helper(
    Graph &g,
    const int v,
    const int child)
{
    // visit indirect descendents of v through child
    for(auto i = 0; i < g.size(); ++i)
    {
        if(g.has_edge(child, i) == false)
            continue;

        // otherwise, there is an indirect path from v to i, remove the
        // direct path from v to i.
        g.remove_edge(v, i);

        transitive_reduce_helper(g, v, i);
    }
}
}

// Ref: https://cs.stackexchange.com/a/29133
template <concepts::DirectedGraph Graph>
constexpr void transitive_reduce(Graph &g)
{
    // perform DFS on all vertices, assuming the graph is a DAG
    for(auto v = 0; v < g.size(); ++v)
    {
        // visit children of v
        for(auto i = 0; i < g.size(); ++i)
        {
            // not child of v
            if(g.has_edge(v, i) == false)
                continue;

            detail::transitive_reduce_helper(g, v, i);
        }
    }
}
}
