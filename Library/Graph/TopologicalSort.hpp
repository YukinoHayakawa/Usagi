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
    typename Traits::template VertexAttributeArray<
        typename Traits::VertexIndexT> &stack,
    std::size_t &cur,
    typename Traits::template VertexAttributeArray<bool> &visited,
    Traits t)
    requires DirectedAcyclicGraph<G, Traits>
{
    visited[v] = true;

    // visit children of v
    for(auto &&c : t.adjacent_vertices(g, v))
    {
        if(!visited[c])
            topological_sort_helper<G, Traits>(g, c, stack, cur, visited, t);
    }

    // ensure the current vertex precedes the children when popping
    // from the stack
    stack[--cur] = v;
}
}

/**
 * \brief Obtain a topological order of the given DAG.
 *
 * Ref: https://www.geeksforgeeks.org/topological-sorting/
 * \tparam G
 * \tparam Traits
 * \param g
 * \param t
 * \return A stack containing one possible topological order of the given
 * DAG. Read the order by popping elements from the stack until it becomes
 * empty.
 */
template <
    typename G,
    typename Traits = typename DefaultGraphTrait<G>::TraitT
>
constexpr auto topological_sort(const G &g, Traits t = { })
    requires DirectedAcyclicGraph<G, Traits>
{
    typename Traits::template VertexAttributeArray<
        typename Traits::VertexIndexT> stack { };
    typename Traits::template VertexAttributeArray<bool> visited { };
    t.resize(stack, t.num_vertices(g));
    t.resize(visited, t.num_vertices(g));

    std::size_t cur = t.num_vertices(g);
    for(auto v = 0; v < t.num_vertices(g); ++v)
        if(!visited[v])
            detail::topological_sort_helper<G, Traits>(
                g, v, stack, cur, visited, t);

    return stack;
}
}
