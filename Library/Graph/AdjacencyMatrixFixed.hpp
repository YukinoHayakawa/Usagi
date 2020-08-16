#pragma once

#include "detail/DefaultGraphTraitStatic.hpp"

namespace usagi::graph
{
template <int Size>
class AdjacencyMatrixFixed
{
    bool matrix[Size][Size] { };
    int in_degree[Size] { };
    int out_degree[Size] { };

public:
    using trait_t = DefaultGraphTraitStatic<AdjacencyMatrixFixed<Size>, Size>;

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
        const int v = -1;
        int i = -1;

        constexpr int operator*() const
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
        const int v = -1;

        constexpr auto begin() const
        {
            auto iter = AdjacentVertexIterator { instance, v, -1 };
            ++iter;
            return iter;
        }

        constexpr auto end() const
        {
            return AdjacentVertexIterator { instance, v, Size };
        }
    };

    constexpr auto adjacent_vertices(const int v) const
    {
        return AdjacentVertexRange { this, v };
        // range-v3 doesn't really work with constexpr
        // return ints(0, Size) | filter([this, v](const int u) constexpr
        // {
            // return has_edge(v, u);
        // });
    }

    /*
     * Edge Operations
     */

    constexpr void add_edge(const int from, const int to)
    {
        matrix[from][to] = 1;
        ++in_degree[to];
        ++out_degree[from];
    }

    constexpr void remove_edge(const int from, const int to)
    {
        matrix[from][to] = 0;
        --in_degree[to];
        --out_degree[from];
    }

    constexpr bool has_edge(const int from, const int to) const
    {
        return matrix[from][to] != 0;
    }

    constexpr bool is_source(const int v) const
    {
        return in_degree[v] == 0;
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
}
