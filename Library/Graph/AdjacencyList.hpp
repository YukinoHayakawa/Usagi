#pragma once

#include <vector>
#include <map>

#include <Usagi/Concept/Type/Graph.hpp>

#include "detail/DefaultGraphTraitDynamic.hpp"

namespace usagi::graph
{
class AdjacencyList
{
public:
    using trait_t = DefaultGraphTraitDynamic<AdjacencyList>;
    using egde_weight_t = float;

private:
    using AdjList = std::map<int, egde_weight_t>;
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

        constexpr int operator*() const
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
        const int v = -1;

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
    };

    auto adjacent_vertices(const int v) const
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

    constexpr void add_edge(
        const int from,
        const int to,
        egde_weight_t weight = 0)
    {
        mAdjacentVertices[from].insert_or_assign(to, weight);
    }

    constexpr void remove_edge(const int from, const int to)
    {
        mAdjacentVertices[from].erase(to);
    }

    constexpr bool has_edge(const int from, const int to) const
    {
        return mAdjacentVertices[from].contains(to);
    }

    /*
     * Edge Weight
     */

    auto edge_weight(const int from, const int to) const
    {
        return mAdjacentVertices[from].find(to)->second;
    }
};

static_assert(concepts::WeightedDirectedAcyclicGraph<AdjacencyList>);
}
