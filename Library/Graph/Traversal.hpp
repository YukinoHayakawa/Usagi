#pragma once

#include <tuple>

#include "Graph.hpp"

namespace usagi::graph
{
namespace detail
{
template <
    typename G,
    typename Traits = typename DefaultGraphTrait<G>::TraitT
>
constexpr void dfs_helper(
    const G &g,
    const typename Traits::VertexIndexT v,
    typename Traits::template VertexAttributeArray<
        typename Traits::VertexIndexT> &output,
    std::size_t &cur,
    typename Traits::template VertexAttributeArray<bool> &visited,
    Traits t)
    requires DirectedGraph<G, Traits>
{
    visited[v] = true;
    output[cur++] = v;

    // visit children of v
    for(auto &&c : t.adjacent_vertices(g, v))
        if(!visited[c])
            dfs_helper<G, Traits>(g, c, output, cur, visited, t);
}
}

template <
    typename G,
    typename Traits = typename DefaultGraphTrait<G>::TraitT
>
constexpr auto dfs(
    const G &g,
    typename Traits::VertexIndexT src,
    Traits t = { })
    requires DirectedGraph<G, Traits>
{
    typename Traits::template VertexAttributeArray<
        typename Traits::VertexIndexT> output { };
    typename Traits::template VertexAttributeArray<bool> visited { };
    // todo unnecessary reservation of space
    t.resize(output, t.num_vertices(g));
    t.resize(visited, t.num_vertices(g));

    std::size_t cur = 0;
    detail::dfs_helper<G, Traits>(g, src, output, cur, visited, t);

    t.resize(output, cur);

    return std::make_tuple(output, cur);
}
}
