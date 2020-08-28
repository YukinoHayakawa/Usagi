#pragma once

namespace usagi::graph
{
/**
 * \brief This trait class allows searching paths through edges by counting the
 * number of passed vertices.
 * \tparam BaseTraits
 * \tparam Weight
 */
template <typename BaseTraits, typename Weight = int>
struct GraphTraitConnectivity : BaseTraits
{
    constexpr GraphTraitConnectivity(BaseTraits base = { })
        : BaseTraits(base)
    {
    }

    using WeightT = Weight;

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
