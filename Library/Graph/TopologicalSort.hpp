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
constexpr void topological_sort_helper(
    const G &g,
    const typename Traits::VertexIndexT v,
    typename Traits::VertexIndexStack &stack,
    typename Traits::template VertexAttributeArray<bool> &visited)
    requires DirectedAcyclicGraph<G, Traits>
{
    Traits t;

    visited[v] = true;

    // visit children of v
    for(auto &&c : t.adjacent_vertices(g, v))
    {
        if(!visited[c])
            topological_sort_helper<G, Traits>(g, c, stack, visited);
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
    typename G,
    typename Traits = typename DefaultGraphTrait<G>::TraitT
>
constexpr auto topological_sort(const G &g)
    requires DirectedAcyclicGraph<G, Traits>
{
    Traits t;

    typename Traits::VertexIndexStack stack;
    typename Traits::template VertexAttributeArray<bool> visited { };
    t.resize(visited, t.num_vertices(g));

    for(auto v = 0; v < t.num_vertices(g); ++v)
        if(!visited[v])
            detail::topological_sort_helper<G, Traits>(g, v, stack, visited);

    return stack;
}
}
