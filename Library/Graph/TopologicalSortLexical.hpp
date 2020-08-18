#pragma once

#include "Level.hpp"
#include "ExtractPath.hpp"

namespace usagi::graph
{
template <
    concepts::DirectedAcyclicGraph Graph,
    typename Traits = typename Graph::trait_t
>
constexpr auto topological_sort_lexical_smallest(const Graph &g)
{
    // todo perf
    auto topo = stack_to_array<Graph, Traits>(
        topological_sort<Graph, Traits>(g)
    );
    const auto lvl = level<Graph, Traits>(g);

    std::sort(topo.begin(), topo.end(), [&](int u, int v) constexpr {
        if(lvl[u] < lvl[v]) return true;
        if(lvl[u] == lvl[v] && u < v) return true;
        return false;
    });

    return topo;
}
}
