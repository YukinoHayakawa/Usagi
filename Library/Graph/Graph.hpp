﻿#pragma once

#include <concepts>
#include <ranges>

#include <Usagi/Concepts/Type/Range.hpp>

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
    // { t.clear_out_edges(g, from) };
};

template <typename T, typename Traits>
concept WeightedGraph = requires(
    T g,
    Traits t,
    typename Traits::VertexIndexT u,
    typename Traits::VertexIndexT v)
{
    typename Traits::WeightT;
    requires std::is_arithmetic_v<typename Traits::WeightT>;
    { t.vertex_weight(g, u) } -> std::same_as<typename Traits::WeightT>;
    { t.edge_weight(g, u, v) } -> std::same_as<typename Traits::WeightT>;
};

template <typename T, typename Traits>
concept WeightedDirectedGraph = requires()
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
auto eval_vertex_weight(V &&v)
    requires std::is_arithmetic_v<std::remove_cvref_t<V>>
{
    return v;
}

template <typename V, typename E>
auto eval_edge_weight(V &&v, E &&e)
    requires std::is_arithmetic_v<std::remove_cvref_t<E>>
{
    return e;
}
}
