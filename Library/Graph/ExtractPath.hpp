#pragma once

#include <Usagi/Concept/Type/Graph.hpp>

namespace usagi::graph
{
/**
 * \brief Given an array containing the pervious vertex of each vertex,
 * track from dest vertex to src vertex to reconstruct a path.
 * \tparam T
 * \param prev
 * \param dest
 * \param src
 * \return A stack containing the reconstructed path. Pop elements from it
 * to read the path from the src vertex to dest vertex.
 */
template <
    concepts::DirectedGraph Graph,
    typename Traits = typename Graph::trait_t
>
auto path_to_stack(
    const typename Traits::template VertexAttributeArray<int> &prev,
    const int src,
    const int dest)
{
    typename Traits::VertexIndexStack stack;

    int cur = dest;
    while(cur != src)
    {
        stack.push(cur);
        cur = prev[cur];
    }
    stack.push(src);

    return stack;
}

template <
    concepts::DirectedGraph Graph,
    typename Traits = typename Graph::trait_t
>
auto stack_to_array(typename Traits::VertexIndexStack &&stack)
{
    Traits traits { stack.size() };
    typename Traits::template VertexAttributeArray<int> array;
    traits.prepare(array);

    std::size_t i = 0;
    while(!stack.empty())
    {
        array[i++] = stack.top();
        stack.pop();
    }

    return array;
}

// todo: we can really use std::deque to avoid this extra work
template <
    concepts::DirectedGraph Graph,
    typename Traits = typename Graph::trait_t
>
auto path_to_array(
    const typename Traits::template VertexAttributeArray<int> &prev,
    const int src,
    const int dest)
{
    return stack_to_array<Graph, Traits>(
        path_to_stack<Graph, Traits>(prev, src, dest)
    );
}
}
