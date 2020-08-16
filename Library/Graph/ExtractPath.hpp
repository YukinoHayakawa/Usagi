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
auto extract_path(
    const typename Traits::VertexAttributeArray<int> &prev,
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
}
