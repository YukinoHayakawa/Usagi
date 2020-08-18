#pragma once

#include <vector>
#include <map>

#include <Usagi/Library/Graph/Graph.hpp>

#include "detail/GraphTraitDynamicSize.hpp"

namespace usagi::graph
{
class AdjacencyList
{
public:
    using EdgeWeightT = double;
    using VertexIndexT = int;
    constexpr static VertexIndexT INVALID_VERTEX_ID = -1;

private:
    using AdjList = std::map<VertexIndexT, EdgeWeightT>;
    std::vector<AdjList> mAdjacentVertices;

public:
    explicit AdjacencyList(int num_vertices)
        : mAdjacentVertices(num_vertices)
    {
    }

    /*
     * Adjacent Vertices
     */

    struct AdjacentVertexIterator
    {
        AdjList::const_iterator iter;

        constexpr VertexIndexT operator*() const
        {
            return iter->first;
        }

        constexpr AdjacentVertexIterator & operator++()
        {
            ++iter;
            return *this;
        }

        constexpr bool operator==(const AdjacentVertexIterator &rhs) const
        {
            return iter == rhs.iter;
        }
    };

    struct AdjacentVertexRange
    {
        const AdjacencyList *instance = nullptr;
        const VertexIndexT v = INVALID_VERTEX_ID;

        auto begin() const
        {
            return AdjacentVertexIterator {
                instance->mAdjacentVertices[v].begin()
            };
        }

        auto end() const
        {
            return AdjacentVertexIterator {
                instance->mAdjacentVertices[v].end()
            };
        }

        std::size_t size() const
        {
            return instance->mAdjacentVertices[v].size();
        }
    };

    auto adjacent_vertices(const VertexIndexT v) const
    {
        return AdjacentVertexRange { this, v };
    }

    std::size_t num_vertices() const
    {
        return mAdjacentVertices.size();
    }

    /*
    * Edge Operations
    */

    void add_edge(
        const VertexIndexT from,
        const VertexIndexT to,
        const EdgeWeightT weight = { })
    {
        mAdjacentVertices[from].insert_or_assign(to, weight);
    }

    void remove_edge(const VertexIndexT from, const VertexIndexT to)
    {
        mAdjacentVertices[from].erase(to);
    }

    bool has_edge(const VertexIndexT from, const VertexIndexT to) const
    {
        return mAdjacentVertices[from].contains(to);
    }

    /*
     * Edge Weight
     */

    void set_edge_weight(
        const VertexIndexT from,
        const VertexIndexT to,
        const EdgeWeightT w)
    {
        mAdjacentVertices[from].find(to)->second = w;
    }

    auto edge_weight(const VertexIndexT from, const VertexIndexT to) const
    {
        return mAdjacentVertices[from].find(to)->second;
    }
};

template <>
struct DefaultGraphTrait<AdjacencyList>
{
    struct Trait
        : GraphTraitWeightedDirected<AdjacencyList>
        , GraphTraitDynamicSize<AdjacencyList>
    {
    };

    using TraitT = Trait;
};

static_assert(WeightedDirectedGraph<
    AdjacencyList,
    DefaultGraphTrait<AdjacencyList>::TraitT
>);
}
