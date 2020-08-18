#pragma once

#include "Graph.hpp"

#include "detail/GraphTraitFixedSize.hpp"

namespace usagi::graph
{
template <int Size>
class AdjacencyMatrixFixed
{
    bool matrix[Size][Size] { };

public:
    using VertexIndexT = int;
    constexpr static VertexIndexT INVALID_VERTEX_ID = -1;

    static constexpr std::size_t num_vertices()
    {
        return Size;
    }

    /*
     * Adjancent Vertex Range
     */

    struct AdjacentVertexIterator
    {
        const AdjacencyMatrixFixed *instance = nullptr;
        const VertexIndexT v = INVALID_VERTEX_ID;
        VertexIndexT i = INVALID_VERTEX_ID;

        constexpr VertexIndexT operator*() const
        {
            return i;
        }

        constexpr AdjacentVertexIterator & operator++()
        {
            while(true)
            {
                ++i;
                if(i >= Size) break;
                if(instance->has_edge(v, i)) break;
            }
            return *this;
        }

        constexpr bool operator==(const AdjacentVertexIterator &rhs) const
        {
            // todo: check iterator compatibility
            return i == rhs.i;
        }
    };

    struct AdjacentVertexRange
    {
        const AdjacencyMatrixFixed *instance = nullptr;
        const VertexIndexT v = INVALID_VERTEX_ID;

        constexpr auto begin() const
        {
            auto iter = AdjacentVertexIterator {
                instance, v, INVALID_VERTEX_ID
            };
            ++iter;
            return iter;
        }

        constexpr auto end() const
        {
            return AdjacentVertexIterator { instance, v, Size };
        }

        constexpr std::size_t size() const
        {
            return Size - std::count(
                instance->matrix[v],
                instance->matrix[v] + Size,
                0
            );
        }
    };

    constexpr auto adjacent_vertices(const VertexIndexT v) const
    {
        return AdjacentVertexRange { this, v };
        // range-v3 doesn't really work with constexpr
        // return ints(0, Size) | filter([this, v](const VertexIndexT u) constexpr
        // {
            // return has_edge(v, u);
        // });
    }

    /*
     * Edge Operations
     */

    constexpr void add_edge(const VertexIndexT from, const VertexIndexT to)
    {
        matrix[from][to] = 1;
    }

    constexpr void remove_edge(const VertexIndexT from, const VertexIndexT to)
    {
        matrix[from][to] = 0;
    }

    constexpr bool has_edge(const VertexIndexT from, const VertexIndexT to) const
    {
        return matrix[from][to] != 0;
    }

    /*
     * Equality Comparator
     */

    constexpr bool equal(const AdjacencyMatrixFixed &rhs) const
    {
        for(auto i = 0; i < Size; ++i)
            for(auto j = 0; j < Size; ++j)
                if(matrix[i][j] != rhs.matrix[i][j])
                    return false;

        return true;
    }

    constexpr AdjacencyMatrixFixed & operator|=(const AdjacencyMatrixFixed &rhs)
    {
        for(auto i = 0; i < Size; ++i)
            for(auto j = 0; j < Size; ++j)
                matrix[i][j] |= rhs.matrix[i][j];

        return *this;
    }

    constexpr bool operator==(const AdjacencyMatrixFixed &rhs) const
    {
        return equal(rhs);
    }

    constexpr bool operator!=(const AdjacencyMatrixFixed &rhs) const
    {
        return !equal(rhs);
    }
};

template <int Size>
struct DefaultGraphTrait<AdjacencyMatrixFixed<Size>>
{
    struct Trait
        : GraphTraitDirected<AdjacencyMatrixFixed<Size>>
        , GraphTraitFixedSize<AdjacencyMatrixFixed<Size>, Size>
    {
    };

    using TraitT = Trait;
};

static_assert(DirectedGraph<
    AdjacencyMatrixFixed<1>,
    DefaultGraphTrait<AdjacencyMatrixFixed<1>>::TraitT
>);
}
