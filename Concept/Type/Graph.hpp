#pragma once

#include <concepts>

namespace usagi::concepts
{
/*
 * Basic Graph Types
 */

// todo: range of vertices
template <typename T>
concept Graph = requires(T t, int v)
{
    typename T::trait_t;
    { t.adjacent_vertices(v) }; // -> range
    { t.num_vertices() } -> std::convertible_to<std::size_t>;
};

template <typename T>
concept DirectedGraph = requires(T t, int from, int to)
{
    { t.add_edge(from, to) };
    { t.remove_edge(from, to) };
    { t.has_edge(from, to) };
} && Graph<T>;

template <typename T>
concept WeightedGraph = requires(T t, int u, int v)
{
    typename T::egde_weight_t;
    requires (std::signed_integral<typename T::egde_weight_t>
        || std::floating_point<typename T::egde_weight_t>);
    { t.edge_weight(u, v) } -> std::same_as<typename T::egde_weight_t>;
};

template <typename T>
concept WeightedDirectedGraph = requires(
    T t,
    int from,
    int to,
    typename T::egde_weight_t weight)
{
    { t.add_edge(from, to, weight) };
    { t.remove_edge(from, to) };
    { t.has_edge(from, to) };
} && Graph<T> && WeightedGraph<T>;

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
