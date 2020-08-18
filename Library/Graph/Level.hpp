#pragma once

#include "Graph.hpp"
#include "FindPath.hpp"
#include "Degree.hpp"

namespace usagi::graph
{
template <
    typename G,
    typename Traits = typename DefaultGraphTrait<G>::TraitT
>
constexpr auto level(const G &g)
    requires DirectedAcyclicGraph<G, Traits>
{
    struct LevelSearchTrait : Traits
    {
        using EdgeWeightT = int;

        constexpr auto set_edge_weight(
            const G &g,
            const typename G::VertexIndexT u,
            const typename G::VertexIndexT v,
            const int w)
        {
            /* no-op */
        }

        constexpr auto edge_weight(
            const G &g,
            const typename Traits::VertexIndexT from,
            const typename Traits::VertexIndexT to) const
        {
            return g.has_edge(from, to) ? 1 : 0;
        }
    };

    auto [prev, lvl, ts] = find_path_dag<G, LevelSearchTrait>(
        g,
        std::numeric_limits<int>::min(),
        std::greater<int>(),
        [&g](auto &&dist) constexpr {
            const auto in_deg = in_degree<G, Traits>(g);
            for(int i = 0; auto deg : in_deg)
            {
                if(deg == 0)
                    dist[i] = 0;
                ++i;
            }
        }
    );

    return std::make_tuple(std::move(lvl), std::move(ts));
}
}
