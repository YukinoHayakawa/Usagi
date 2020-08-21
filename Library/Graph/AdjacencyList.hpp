﻿#pragma once

#include <vector>
#include <map>

#include <Usagi/Library/Graph/Graph.hpp>

#include "detail/GraphTraitDynamicSize.hpp"

namespace usagi::graph
{
template <typename Vertex = double, typename Edge = double>
class AdjacencyList
{
public:
    using VertexT = Vertex;
    using EdgeT = Edge;

    using VertexIndexT = int;
    constexpr static VertexIndexT INVALID_VERTEX_ID = -1;
    using VertexWeightT = decltype(graph::vertex_weight(
        std::declval<VertexT>())
    );
    using EdgeWeightT = decltype(graph::edge_weight(
        std::declval<VertexT>(), std::declval<EdgeT>())
    );

private:
    using AdjacentVertices = std::map<VertexIndexT, EdgeT>;

    struct VertexInfo
    {
        AdjacentVertices adj;
        VertexT data;
    };
    std::vector<VertexInfo> mVertices;

public:
    explicit AdjacencyList(VertexIndexT num_vertices)
        : mVertices(num_vertices)
    {
    }

    /*
     * Adjacent Vertices
     */

    struct AdjacentVertexIterator
    {
        typename AdjacentVertices::const_iterator iter;

        VertexIndexT operator*() const
        {
            return iter->first;
        }

        auto & edge()
        {
            return iter->second;
        }

        AdjacentVertexIterator & operator++()
        {
            ++iter;
            return *this;
        }

        bool operator==(const AdjacentVertexIterator &rhs) const
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
                instance->mVertices[v].adj.begin()
            };
        }

        auto end() const
        {
            return AdjacentVertexIterator {
                instance->mVertices[v].adj.end()
            };
        }

        std::size_t size() const
        {
            return instance->mVertices[v].adj.size();
        }
    };

    auto adjacent_vertices(const VertexIndexT v) const
    {
        return AdjacentVertexRange { this, v };
    }

    std::size_t num_vertices() const
    {
        return mVertices.size();
    }

    /*
     * Edge Operations
     */

    void add_edge(
        const VertexIndexT from,
        const VertexIndexT to,
        const EdgeT edge = { })
    {
        mVertices[from].adj.insert_or_assign(to, edge);
    }

    void remove_edge(const VertexIndexT from, const VertexIndexT to)
    {
        mVertices[from].adj.erase(to);
    }

    bool has_edge(const VertexIndexT from, const VertexIndexT to) const
    {
        return mVertices[from].adj.contains(to);
    }

    /*
     * Vertex Data/Weight
     */

    auto & vertex(const VertexIndexT v)
    {
       return mVertices[v].data;
    }

    auto vertex_weight(const VertexIndexT v) const
    {
        const auto &vtx = mVertices[v];
        return graph::vertex_weight(vtx.data);
    }

    /*
     * Edge Data/Weight
     */

    auto & edge(const VertexIndexT from, const VertexIndexT to)
    {
        return mVertices[from].find(to)->second;
    }

    auto edge_weight(const VertexIndexT from, const VertexIndexT to) const
    {
        const auto &vtx = mVertices[from];
        return graph::edge_weight(
            vtx.data,
            vtx.adj.find(to)->second
        );
    }
};

template <>
struct DefaultGraphTrait<AdjacencyList<>>
{
    struct Trait
        : GraphTraitWeightedDirected<AdjacencyList<>>
        , GraphTraitDynamicSize<AdjacencyList<>>
    {
    };

    using TraitT = Trait;
};

static_assert(WeightedDirectedGraph<
    AdjacencyList<>,
    DefaultGraphTrait<AdjacencyList<>>::TraitT
>);
}
