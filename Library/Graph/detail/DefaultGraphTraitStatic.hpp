#pragma once

#include <Usagi/Library/Meta/Stack.hpp>

namespace usagi::graph
{
// Cannot require concept Graph on G since it would cause circular depencency
// when defining trait_t, which is required by concept Graph.
template <typename G, std::size_t Size>
struct DefaultGraphTraitStatic
{
    template <typename T>
    using VertexAttributeArray = std::array<T, Size>;

    template <typename T>
    static constexpr void prepare(VertexAttributeArray<T> &)
    {
        /* no-op */
    }

    using VertexIndexStack = meta::Stack<Size>;

    constexpr DefaultGraphTraitStatic() = default;

    explicit constexpr DefaultGraphTraitStatic(const G &g)
    {
    }

    explicit constexpr DefaultGraphTraitStatic(const std::size_t size)
    {
    }

    static constexpr auto edge_weight(
        const G &g,
        int u,
        int v)
    {
        return g.edge_weight(u, v);
    }
};
}
