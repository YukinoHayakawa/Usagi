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

    static constexpr auto adjacent_vertices(const G &g, VertexIndexT v)
    {
        return g.adjacent_vertices(v);
    }

    static constexpr auto num_vertices(const G &g)
    {
        return g.num_vertices();
    }

    static constexpr auto has_edge(const G &g, VertexIndexT u, VertexIndexT v)
    {
        return g.has_edge(u, v);
    }
};

template <typename G>
struct GraphTraitDirected : GraphTrait<G>
{
    static constexpr auto add_edge(
        G &g,
        typename GraphTrait<G>::VertexIndexT u,
        typename GraphTrait<G>::VertexIndexT v)
    {
        return g.add_edge(u, v);
    }

    static constexpr auto remove_edge(
        G &g,
        typename GraphTrait<G>::VertexIndexT u,
        typename GraphTrait<G>::VertexIndexT v)
    {
        return g.remove_edge(u, v);
    }

    /*
    static constexpr auto clear_out_edges(
        G &g,
        typename GraphTrait<G>::VertexIndexT u)
    {
        return g.clear_out_edges(u);
    }
    */
};

template <typename G>
struct GraphTraitWeighted
{
    using WeightT = typename G::WeightT;

    static constexpr auto vertex_weight(
        const G &g,
        const typename G::VertexIndexT u)
    {
        return g.vertex_weight(u);
    }

    static constexpr auto edge_weight(
        const G &g,
        const typename G::VertexIndexT u,
        const typename G::VertexIndexT v)
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
