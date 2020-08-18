#pragma once

#include <Usagi/Concept/Type/Graph.hpp>

namespace usagi::graph
{
template <
    concepts::DirectedAcyclicGraph Graph,
    typename Traits = typename Graph::trait_t
>
constexpr auto out_degree(const Graph &g)
{
    auto traits = Traits(g);

    typename Traits::template VertexAttributeArray<int> out_deg { };
    traits.prepare(out_deg);

    for(auto v = 0; v < g.num_vertices(); ++v)
        out_deg[v] = g.adjacent_vertices(v).size();

    return out_deg;
}

template <
    concepts::DirectedAcyclicGraph Graph,
    typename Traits = typename Graph::trait_t
>
constexpr auto in_degree(const Graph &g)
{
    auto traits = Traits(g);

    typename Traits::template VertexAttributeArray<int> in_deg { };
    traits.prepare(in_deg);

    for(auto v = 0; v < g.num_vertices(); ++v)
        for(auto c : g.adjacent_vertices(v))
            ++in_deg[c];

    return in_deg;
}
}
