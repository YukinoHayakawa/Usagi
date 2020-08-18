#pragma once

#include "Level.hpp"
#include "ExtractPath.hpp"

namespace usagi::graph
{
template <
    typename G,
    typename Traits = typename DefaultGraphTrait<G>::TraitT
>
constexpr auto topological_sort_lexical_smallest(const G &g)
    requires DirectedAcyclicGraph<G, Traits>
{
    // todo perf
    auto topo = stack_to_array<G, Traits>(g,
        topological_sort<G, Traits>(g)
    );
    const auto lvl = level<G, Traits>(g);

    std::sort(topo.begin(), topo.end(), [&](
        const typename Traits::VertexIndexT u,
        const typename Traits::VertexIndexT v
    ) constexpr {
        if(lvl[u] < lvl[v]) return true;
        if(lvl[u] == lvl[v] && u < v) return true;
        return false;
    });

    return topo;
}
}
