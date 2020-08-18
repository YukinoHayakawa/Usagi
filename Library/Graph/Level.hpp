#pragma once

#include <Usagi/Concept/Type/Graph.hpp>

#include "FindPath.hpp"
#include "Degree.hpp"

namespace usagi::graph
{
template <
    concepts::DirectedAcyclicGraph Graph,
    typename Traits = typename Graph::trait_t
>
constexpr auto level(const Graph &g)
{
    struct LevelSearchTrait : Traits
    {
        const Graph &g;

        constexpr explicit LevelSearchTrait(const Graph &g)
            : Traits(g)
            , g(g)
        {
        }

        constexpr auto edge_weight(const Graph &g, int u, int v) const
        {
            return g.has_edge(u, v) ? 1 : 0;
        }
    };

    auto [prev, dist] = find_path_dag<Graph, LevelSearchTrait, int>(
        g,
        std::numeric_limits<int>::min(),
        std::greater<int>(),
        [&g](auto &&dist) constexpr {
            const auto in_deg = in_degree<Graph, Traits>(g);
            for(int i = 0; auto deg : in_deg)
            {
                if(deg == 0)
                    dist[i] = 0;
                ++i;
            }
        }
    );
    return dist;
}
}
