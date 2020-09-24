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
 * \tparam G
 * \tparam Traits
 * \param g
 * \param init_dist
 * \param cmp
 * \param source
 * \param t
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
    const typename Traits::WeightT init_dist,
    Comparator cmp,
    Source source,
    Traits t = { })
    requires WeightedDirectedAcyclicGraph<G, Traits>
{
    typename Traits::template VertexAttributeArray<
        typename Traits::VertexIndexT> prev { };
    typename Traits::template VertexAttributeArray<
        typename Traits::WeightT> dist { };
    t.resize(prev, t.num_vertices(g));
    t.resize(dist, t.num_vertices(g));
    std::fill(prev.begin(), prev.end(), Traits::INVALID_VERTEX_ID);
    std::fill(dist.begin(), dist.end(), init_dist);
    source(dist);

    const auto ts = topological_sort<G, Traits>(g, t);
    for(auto v : ts)
    {
        // start from the source vertex
        if(dist[v] == init_dist) continue;
        // visit children of v
        for(auto &&u : t.adjacent_vertices(g, v))
        {
            const auto alt_path = dist[v] +
                t.vertex_weight(g, v) + t.edge_weight(g, v, u);
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
    typename Traits = typename DefaultGraphTrait<G>::TraitT,
    typename Comparator,
    typename Source
>
constexpr auto find_path_all_dag(
    const G &g,
    G &out, // empty graph for storing the result
    const typename Traits::WeightT init_dist,
    Comparator cmp,
    Source source,
    Traits t = { })
    requires WeightedDirectedAcyclicGraph<G, Traits>
{
    auto [prev, dist, ts] = find_path_dag<G, Traits, Comparator, Source>(
        g, init_dist, std::move(cmp), std::move(source), t
    );

    // add all reserve edges from the path search to the output graph
    for(std::size_t i = 0; i < t.num_vertices(g); ++i)
        if(prev[i] != Traits::INVALID_VERTEX_ID)
            t.add_edge(out, i, prev[i]);

    // visit all edges and insert edges that make paths of the same length.
    // this works because the find_path_dag retains connectivity from source
    // to other vertices. however, some edges making parallel longest paths
    // were dropped because prev can only store one edge. this process
    // basically restores those dropped edges.
    for(std::size_t v = 0; v < t.num_vertices(g); ++v)
        for(auto &&c : t.adjacent_vertices(g, v)) // visit edge v->c
            // edge of the same length but discarded in the single path search
            if(dist[v] + t.vertex_weight(g, v) + t.edge_weight(g, v, c)
                == dist[c])
                t.add_edge(out, c, v); // add it back

    return std::make_tuple(std::move(prev), std::move(dist), std::move(ts));
}

template <
    typename G,
    typename Traits = typename DefaultGraphTrait<G>::TraitT
>
constexpr decltype(auto) longest_path_dag(
    const G &g,
    const typename Traits::VertexIndexT source,
    Traits t = { })
    requires WeightedDirectedAcyclicGraph<G, Traits>
{
    return find_path_dag<G, Traits>(
        g,
        std::numeric_limits<typename Traits::WeightT>::lowest(),
        std::greater<typename Traits::WeightT>(),
        [source](auto &&dist) constexpr {
            dist[source] = typename Traits::WeightT { };
        },
        t
    );
}

template <
    typename G,
    typename Traits = typename DefaultGraphTrait<G>::TraitT
>
constexpr decltype(auto) longest_path_all_dag(
    const G &g,
    G &out,
    const typename Traits::VertexIndexT source,
    Traits t = { })
    requires WeightedDirectedAcyclicGraph<G, Traits>
{
    return find_path_all_dag<G, Traits>(
        g,
        out,
        std::numeric_limits<typename Traits::WeightT>::lowest(),
        std::greater<typename Traits::WeightT>(),
        [source](auto &&dist) constexpr {
            dist[source] = typename Traits::WeightT { };
        },
        t
    );
}

template <
    typename G,
    typename Traits = typename DefaultGraphTrait<G>::TraitT
>
constexpr decltype(auto) shortest_path_dag(
    const G &g,
    const typename Traits::VertexIndexT source,
    Traits t = { })
    requires WeightedDirectedAcyclicGraph<G, Traits>
{
    return find_path_dag<G, Traits>(
        g,
        std::numeric_limits<typename Traits::WeightT>::max(),
        std::less<typename Traits::WeightT>(),
        [source](auto &&dist) constexpr {
            dist[source] = typename Traits::WeightT { };
        },
        t
    );
}
}
