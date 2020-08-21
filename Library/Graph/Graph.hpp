#pragma once

#include <concepts>
#include <ranges>

#include <Usagi/Concept/Type/Range.hpp>

namespace usagi::graph
{
/*
 * Basic Graph Types
 */

template <typename T, typename Traits>
concept Graph = requires(
    T g,
    Traits t,
    typename Traits::VertexIndexT u,
    typename Traits::VertexIndexT v)
{
    typename Traits::GraphT;
    typename Traits::VertexIndexT;
    { Traits::INVALID_VERTEX_ID } ->
        std::same_as<const typename Traits::VertexIndexT>;
    { t.adjacent_vertices(g, v) } -> concepts::SizedRange;
    { t.num_vertices(g) } -> std::convertible_to<std::size_t>;
    { t.has_edge(g, v, u) };
};

template <typename T, typename Traits>
concept DirectedGraph = requires(
    T g,
    Traits t,
    typename Traits::VertexIndexT from,
    typename Traits::VertexIndexT to)
{
    requires Graph<T, Traits>;
    { t.add_edge(g, from, to) };
    { t.remove_edge(g, from, to) };
};

template <typename T, typename Traits>
concept WeightedGraph = requires(
    T g,
    Traits t,
    typename Traits::VertexIndexT u,
    typename Traits::VertexIndexT v,
    typename Traits::EdgeWeightT w)
{
    typename Traits::VertexWeightT;
    typename Traits::EdgeWeightT;
    requires std::is_signed_v<typename Traits::VertexWeightT>;
    requires std::is_signed_v<typename Traits::EdgeWeightT>;
    { t.vertex_weight(g, u) } -> std::same_as<typename Traits::VertexWeightT>;
    { t.edge_weight(g, u, v) } -> std::same_as<typename Traits::EdgeWeightT>;
};

template <typename T, typename Traits>
concept WeightedDirectedGraph = requires(
    T g,
    Traits t,
    typename Traits::VertexIndexT from,
    typename Traits::VertexIndexT to,
    typename Traits::EdgeWeightT weight)
{
    requires DirectedGraph<T, Traits>;
    requires WeightedGraph<T, Traits>;
};

template <typename G>
struct DefaultGraphTrait;

/*
 * DAG...
 *
 * Actually we can't really apply compile time checks for cycles in a graph
 * in general. These concepts are more like reminders for checking the
 * parameters before passing a graph to an algorithm.
 */

template <typename G, typename Traits>
concept DirectedAcyclicGraph = DirectedGraph<G, Traits>;

template <typename G, typename Traits>
concept WeightedDirectedAcyclicGraph = WeightedDirectedGraph<G, Traits>;

template <typename V>
auto vertex_weight(V &&v)
{
    return v;
}

template <typename V, typename E>
auto edge_weight(V &&v, E &&e)
{
    return e;
}
}
