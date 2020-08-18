#pragma once

#include <concepts>
#include <ranges>

#include "Range.hpp"

namespace usagi::concepts
{
/*
 * Basic Graph Types
 */

template <typename T>
concept Graph = requires(T t, int v, int from, int to)
{
    typename T::trait_t;
    { t.adjacent_vertices(v) } -> SizedRange;
    { t.num_vertices() } -> std::convertible_to<std::size_t>;
    { t.has_edge(from, to) };
};

template <typename T>
concept DirectedGraph = requires(T t, int from, int to)
{
    requires Graph<T>;
    { t.add_edge(from, to) };
    { t.remove_edge(from, to) };
};

template <typename T>
concept WeightedGraph = requires(T t, int u, int v)
{
    typename T::edge_weight_t;
    requires (std::signed_integral<typename T::edge_weight_t>
        || std::floating_point<typename T::edge_weight_t>);
    { t.edge_weight(u, v) } -> std::same_as<typename T::edge_weight_t>;
};

template <typename T>
concept WeightedDirectedGraph = requires(
    T t,
    int from,
    int to,
    typename T::edge_weight_t weight)
{
    requires Graph<T>;
    requires WeightedGraph<T>;
    { t.add_edge(from, to, weight) };
    { t.remove_edge(from, to) };
    { t.has_edge(from, to) };
};

/*
 * DAG...
 *
 * Actually we can't really apply compile time checks for cycles in a graph
 * in general. These concepts are more like reminders for checking the
 * parameters before passing a graph to an algorithm.
 */

template <typename T>
concept DirectedAcyclicGraph = DirectedGraph<T>;

template <typename T>
concept WeightedDirectedAcyclicGraph = WeightedDirectedGraph<T>;
}
