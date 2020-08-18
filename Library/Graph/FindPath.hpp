#pragma once

#include <tuple>
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
    typename G,
    typename Traits = typename DefaultGraphTrait<G>::TraitT,
    typename Comparator,
    typename Source
>
constexpr auto find_path_dag(
    const G &g,
    const typename Traits::EdgeWeightT init_dist,
    Comparator cmp,
    Source source)
    requires WeightedDirectedAcyclicGraph<G, Traits>
{
    Traits t;

    typename Traits::template VertexAttributeArray<
        typename Traits::VertexIndexT> prev { };
    typename Traits::template VertexAttributeArray<
        typename Traits::EdgeWeightT> dist { };
    t.resize(prev, t.num_vertices(g));
    t.resize(dist, t.num_vertices(g));
    std::fill(
        dist.begin(),
        dist.end(),
        init_dist
    );
    source(dist);

    const auto ts = topological_sort<G, Traits>(g);
    for(auto v : ts)
    {
        // start from the source vertex
        if(dist[v] == init_dist) continue;
        // visit children of v
        for(auto &&u : t.adjacent_vertices(g, v))
        {
            const auto alt_path = dist[v] + t.edge_weight(g, v, u);
            // found a longer/shorter path via v to u
            if(cmp(alt_path, dist[u]))
            {
                dist[u] = alt_path;
                prev[u] = v;
            }
        }
    }

    return std::make_tuple(std::move(prev), std::move(dist), std::move(ts));
}

template <
    typename G,
    typename Traits = typename DefaultGraphTrait<G>::TraitT
>
constexpr decltype(auto) longest_path_dag(
    const G &g,
    const typename Traits::VertexIndexT source)
    requires WeightedDirectedAcyclicGraph<G, Traits>
{
    return find_path_dag<G, Traits>(
        g,
        std::numeric_limits<typename Traits::EdgeWeightT>::min(),
        std::greater<typename Traits::EdgeWeightT>(),
        [source](auto &&dist) constexpr {
            dist[source] = typename Traits::VertexIndexT { };
        }
    );
}

template <
    typename G,
    typename Traits = typename DefaultGraphTrait<G>::TraitT
>
constexpr decltype(auto) shortest_path_dag(
    const G &g,
    const typename Traits::VertexIndexT source)
    requires WeightedDirectedAcyclicGraph<G, Traits>
{
    return find_path_dag<G, Traits>(
        g,
        std::numeric_limits<typename Traits::EdgeWeightT>::max(),
        std::less<typename Traits::EdgeWeightT>(),
        [source](auto &&dist) constexpr {
            dist[source] = typename Traits::VertexIndexT { };
        }
    );
}
}
