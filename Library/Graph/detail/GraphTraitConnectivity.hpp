#pragma once

namespace usagi::graph
{
template <typename BaseTraits, typename Weight = int>
struct GraphTraitConnectivity : BaseTraits
{
    using VertexWeightT = Weight;
    using EdgeWeightT = Weight;

    constexpr auto vertex_weight(
        const typename BaseTraits::GraphT &g,
        const typename BaseTraits::VertexIndexT u) const
    {
        return 1;
    }

    constexpr auto edge_weight(
        const typename BaseTraits::GraphT &g,
        const typename BaseTraits::VertexIndexT from,
        const typename BaseTraits::VertexIndexT to) const
    {
        return 0;
    }
};
}
