#pragma once

#include <Usagi/Concept/Type/Graph.hpp>

namespace usagi::graph
{
namespace detail
{
template <
    concepts::DirectedAcyclicGraph Graph,
    typename Traits
>
constexpr void topological_sort_helper(
    const Graph &g,
    const int v,
    typename Traits::VertexIndexStack &stack,
    typename Traits::template VertexAttributeArray<bool> &visited)
{
    visited[v] = true;

    // visit children of v
    for(auto &&c : g.adjacent_vertices(v))
    {
        if(!visited[c])
            topological_sort_helper<Graph, Traits>(g, c, stack, visited);
    }

    // ensure the current vertex precedes the children when popping
    // from the stack
    stack.push(v);
}
}

/**
 * \brief Obtain a topological order of the given DAG.
 *
 * Ref: https://www.geeksforgeeks.org/topological-sorting/
 * \tparam Graph
 * \tparam Traits
 * \param g
 * \return A stack containing one possible topological order of the given
 * DAG. Read the order by popping elements from the stack until it becomes
 * empty.
 */
template <
    concepts::DirectedAcyclicGraph Graph,
    typename Traits = typename Graph::trait_t
>
constexpr auto topological_sort(const Graph &g)
{
    auto traits = Traits(g);

    typename Traits::VertexIndexStack stack;
    typename Traits::template VertexAttributeArray<bool> visited { };
    traits.prepare(visited);

    for(auto v = 0; v < g.num_vertices(); ++v)
        if(!visited[v])
            detail::topological_sort_helper<Graph, Traits>(
                g, v, stack, visited);

    return stack;
}
}
