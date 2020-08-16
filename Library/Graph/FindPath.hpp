#pragma once

#include <limits>

#include "TopologicalSort.hpp"

namespace usagi::graph
{
/**
 * \brief Finds a longest/shortest path from source to other vertices.
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
    typename Traits = typename Graph::trait_t,
    typename Comparator
>
constexpr auto find_path_dag(
    const Graph &g,
    const int source,
    typename Graph::egde_weight_t init_dist,
    Comparator cmp)
{
    auto traits = Traits(g);

    typename Traits::VertexAttributeArray<int> prev { };
    typename Traits::VertexAttributeArray<typename Graph::egde_weight_t> dist{};
    traits.prepare(prev);
    traits.prepare(dist);
    std::fill(
        dist.begin(),
        dist.end(),
        init_dist
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
            // found a longer/shorter path via v to u
            if(cmp(alt_path, dist[u]))
            {
                dist[u] = alt_path;
                prev[u] = v;
            }
        }
    }

    return prev;
}

template <
    concepts::WeightedDirectedAcyclicGraph Graph,
    typename Traits = typename Graph::trait_t
>
constexpr decltype(auto) longest_path_dag(const Graph &g, const int source)
{
    return find_path_dag<Graph, Traits>(
        g,
        source,
        std::numeric_limits<typename Graph::egde_weight_t>::min(),
        std::greater<typename Graph::egde_weight_t>()
    );
}

template <
    concepts::WeightedDirectedAcyclicGraph Graph,
    typename Traits = typename Graph::trait_t
>
constexpr decltype(auto) shortest_path_dag(const Graph &g, const int source)
{
    return find_path_dag<Graph, Traits>(
        g,
        source,
        std::numeric_limits<typename Graph::egde_weight_t>::max(),
        std::less<typename Graph::egde_weight_t>()
    );
}
}
