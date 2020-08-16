#pragma once

#include <limits>

#include "TopologicalSort.hpp"

namespace usagi::graph
{
/**
 * \brief Finds the longest paths from source to other vertices.
 *
 * Ref: https://www.geeksforgeeks.org/find-longest-path-directed-acyclic-graph/
 * \tparam Graph
 * \tparam Traits
 * \param g
 * \param source
 * \return An array of parent vertex of each vertex that can be used to extract
 * the path from source to another vertex.
 */
template <
    concepts::WeightedDirectedAcyclicGraph Graph,
    typename Traits = typename Graph::trait_t
>
constexpr auto longest_path_dag(const Graph &g, const int source)
{
    auto traits = Traits(g);

    typename Traits::VertexAttributeArray<int> prev { };
    typename Traits::VertexAttributeArray<typename Graph::egde_weight_t> dist{};
    traits.prepare(prev);
    traits.prepare(dist);
    std::fill(
        dist.begin(),
        dist.end(),
        std::numeric_limits<Graph::egde_weight_t>::min()
    );
    dist[source] = 0;

    auto ts = topological_sort(g);

    while(!ts.empty())
    {
        const auto v = ts.top();
        ts.pop();
        // visit children of v
        for(auto &&u : g.adjacent_vertices(v))
        {
            const auto alt_path = dist[v] + g.edge_weight(v, u);
            // found a longer path via v to u
            if(dist[u] < alt_path)
            {
                dist[u] = alt_path;
                prev[u] = v;
            }
        }
    }

    return prev;
}
}
