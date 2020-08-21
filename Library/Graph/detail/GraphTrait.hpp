#pragma once

namespace usagi::graph
{
// Cannot require concept Graph on G since it would cause circular depencency
// when defining trait_t, which is required by concept Graph.
template <typename G>
struct GraphTrait
{
    using GraphT = G;
    using VertexIndexT = typename G::VertexIndexT;
    constexpr static VertexIndexT INVALID_VERTEX_ID = G::INVALID_VERTEX_ID;

    constexpr auto adjacent_vertices(const G &g, VertexIndexT v) const
    {
        return g.adjacent_vertices(v);
    }

    constexpr auto num_vertices(const G &g) const
    {
        return g.num_vertices();
    }

    constexpr auto has_edge(const G &g, VertexIndexT u, VertexIndexT v) const
    {
        return g.has_edge(u, v);
    }
};

template <typename G>
struct GraphTraitDirected : GraphTrait<G>
{
    constexpr auto add_edge(G &g, VertexIndexT u, VertexIndexT v) const
    {
        return g.add_edge(u, v);
    }

    constexpr auto remove_edge(G &g, VertexIndexT u, VertexIndexT v) const
    {
        return g.remove_edge(u, v);
    }
};

template <typename G>
struct GraphTraitWeighted
{
    using VertexWeightT = typename G::VertexWeightT;
    using EdgeWeightT = typename G::EdgeWeightT;

    constexpr auto vertex_weight(
        const G &g,
        const typename G::VertexIndexT u) const
    {
        return g.vertex_weight(u);
    }

    constexpr auto edge_weight(
        const G &g,
        const typename G::VertexIndexT u,
        const typename G::VertexIndexT v) const
    {
        return g.edge_weight(u, v);
    }
};

template <typename G>
struct GraphTraitWeightedDirected
    : GraphTraitDirected<G>
    , GraphTraitWeighted<G>
{

};
}
