#pragma once

#include "Graph.hpp"
#include "FindPath.hpp"
#include "Degree.hpp"

#include "detail/GraphTraitConnectivity.hpp"

namespace usagi::graph
{
template <
    typename G,
    typename Traits = typename DefaultGraphTrait<G>::TraitT
>
constexpr auto level(const G &g)
    requires DirectedAcyclicGraph<G, Traits>
{
    auto [prev, lvl, ts] = find_path_dag<G, GraphTraitConnectivity<Traits>>(
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

    return std::make_tuple(std::move(prev), std::move(lvl), std::move(ts));
}
}
