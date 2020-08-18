#pragma once

#include "Graph.hpp"

namespace usagi::graph
{
template <
    typename G,
    typename Traits = typename DefaultGraphTrait<G>::TraitT
>
constexpr auto path_length(
    const typename Traits::template VertexAttributeArray<
        typename Traits::VertexIndexT> &prev,
    const typename Traits::VertexIndexT src,
    const typename Traits::VertexIndexT dest)
    requires Graph<G, Traits>
{
    std::size_t i = 1; // dest

    typename Traits::VertexIndexT cur = dest;
    while(cur != src)
    {
        cur = prev[cur];
        ++i;
    }

    return i;
}

/*
/**
 * \brief Given an array containing the pervious vertex of each vertex,
 * track from dest vertex to src vertex to reconstruct a path.
 * \tparam T
 * \param prev
 * \param dest
 * \param src
 * \return A stack containing the reconstructed path. Pop elements from it
 * to read the path from the src vertex to dest vertex.
 #1#
template <
    typename G,
    typename Traits = typename DefaultGraphTrait<G>::TraitT
>
constexpr auto path_to_stack(
    const typename Traits::template VertexAttributeArray<
        typename Traits::VertexIndexT> &prev,
    const typename Traits::VertexIndexT src,
    const typename Traits::VertexIndexT dest)
    requires DirectedGraph<G, Traits>
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
*/

/*
template <
    typename G,
    typename Traits = typename DefaultGraphTrait<G>::TraitT
>
constexpr auto stack_to_array(
    const G &g,
    typename Traits::VertexIndexStack stack)
    requires DirectedGraph<G, Traits>
{
    Traits t;
    typename Traits::template VertexAttributeArray<
        typename Traits::VertexIndexT> array;
    t.resize(array, stack.size());

    std::size_t i = 0;
    while(!stack.empty())
    {
        array[i++] = stack.top();
        stack.pop();
    }

    return array;
}
*/

template <
    typename G,
    typename Traits = typename DefaultGraphTrait<G>::TraitT
>
constexpr auto path_to_array(
    const typename Traits::template VertexAttributeArray<
        typename Traits::VertexIndexT> &prev,
    const typename Traits::VertexIndexT src,
    const typename Traits::VertexIndexT dest)
    requires DirectedGraph<G, Traits>
{
    Traits t;

    auto length = path_length<G, Traits>(prev, src, dest);
    typename Traits::template VertexAttributeArray<
        typename Traits::VertexIndexT> array;
    t.resize(array, length);

    // use the array like a stack
    int cur = dest;
    while(cur != src)
    {
        array[--length] = cur;
        cur = prev[cur];
    }
    array[--length] = cur;

    return array;
}
}
