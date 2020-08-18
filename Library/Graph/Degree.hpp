#pragma once

#include "Graph.hpp"

namespace usagi::graph
{
template <
    typename G,
    typename Traits = typename DefaultGraphTrait<G>::TraitT
>
constexpr auto out_degree(const G &g)
    requires DirectedAcyclicGraph<G, Traits>
{
    Traits t;

    typename Traits::template VertexAttributeArray<
        typename Traits::VertexIndexT> out_deg { };
    t.resize(out_deg, t.num_vertices(g));

    for(auto v = 0; v < t.num_vertices(g); ++v)
        out_deg[v] = t.adjacent_vertices(g, v).size();

    return out_deg;
}

template <
    typename G,
    typename Traits = typename DefaultGraphTrait<G>::TraitT
>
constexpr auto in_degree(const G &g)
    requires DirectedAcyclicGraph<G, Traits>
{
    Traits t;

    typename Traits::template VertexAttributeArray<
        typename Traits::VertexIndexT> in_deg { };
    t.resize(in_deg, t.num_vertices(g));

    for(auto v = 0; v < t.num_vertices(g); ++v)
        for(auto c : t.adjacent_vertices(g, v))
            ++in_deg[c];

    return in_deg;
}
}
