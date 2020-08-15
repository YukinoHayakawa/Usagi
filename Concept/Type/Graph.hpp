#pragma once

#include <concepts>

namespace usagi::concepts
{
template <typename T>
concept Graph = requires(T t, int v)
{
    { t.adjacent_vertices(v) }; // return a range
    { t.num_vertices() } -> std::convertible_to<std::size_t>;
};

template <typename T>
concept DirectedGraph = requires(T t, int from, int to)
{
    { t.add_edge(from, to) };
    { t.remove_edge(from, to) };
    { t.has_edge(from, to) };
} && Graph<T>;

// We can't really apply compile time checks for cycles in a graph.
template <typename T>
concept DirectedAcyclicGraph = DirectedGraph<T>;
}
