#pragma once

#include <concepts>

namespace usagi::concepts
{
template <typename T>
concept DirectedGraph = requires(T t, int from, int to)
{
    { t.add_edge(from, to) };
    { t.remove_edge(from, to) };
    { t.has_edge(from, to) };
    { t.size() } -> std::convertible_to<std::size_t>;
};
}
