#pragma once

#include <algorithm>

#include "Level.hpp"

namespace usagi::graph
{
template <
    typename G,
    typename Traits = typename DefaultGraphTrait<G>::TraitT
>
constexpr auto topological_sort_lexical_smallest(const G &g, Traits t = { })
    requires DirectedAcyclicGraph<G, Traits>
{
    auto [_, lvl, topo] = level<G, Traits>(g, t);

    std::sort(topo.begin(), topo.end(), [&lvl = lvl](
        const typename Traits::VertexIndexT u,
        const typename Traits::VertexIndexT v
    ) constexpr {
        // return a lexicographic order by level then index
        return std::tie(lvl[u], u) < std::tie(lvl[v], v);
    });

    return topo;
}
}
