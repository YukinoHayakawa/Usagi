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
 * the path from source to another vertex and an array of longest/shortest
 * path length from the source to each vertex.
 */
template <
    concepts::DirectedAcyclicGraph Graph,
    typename Traits = typename Graph::trait_t,
    typename EdgeWeight = typename Graph::edge_weight_t,
    typename Comparator,
    typename Source
>
constexpr auto find_path_dag(
    const Graph &g,
    EdgeWeight init_dist,
    Comparator cmp,
    Source source)
{
    auto traits = Traits(g);

    typename Traits::template VertexAttributeArray<int> prev { };
    typename Traits::template VertexAttributeArray<EdgeWeight> dist { };
    traits.prepare(prev);
    traits.prepare(dist);
    std::fill(
        dist.begin(),
        dist.end(),
        init_dist
    );
    source(dist);

    auto ts = topological_sort(g);

    while(!ts.empty())
    {
        const auto v = ts.top();
        ts.pop();
        // start from the source vertex
        if(dist[v] == init_dist) continue;
        // visit children of v
        for(auto &&u : g.adjacent_vertices(v))
        {
            const auto alt_path = dist[v] + traits.edge_weight(g, v, u);
            // found a longer/shorter path via v to u
            if(cmp(alt_path, dist[u]))
            {
                dist[u] = alt_path;
                prev[u] = v;
            }
        }
    }

    return std::make_tuple(prev, dist);
}

template <
    concepts::WeightedDirectedAcyclicGraph Graph,
    typename Traits = typename Graph::trait_t
>
constexpr decltype(auto) longest_path_dag(const Graph &g, const int source)
{
    return find_path_dag<Graph, Traits>(
        g,
        std::numeric_limits<typename Graph::edge_weight_t>::min(),
        std::greater<typename Graph::edge_weight_t>(),
        [source](auto &&dist) constexpr { dist[source] = 0; }
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
        std::numeric_limits<typename Graph::edge_weight_t>::max(),
        std::less<typename Graph::edge_weight_t>(),
        [source](auto &&dist) constexpr { dist[source] = 0; }
    );
}
}
