#pragma once

namespace usagi
{
template <int Size>
struct AdjacencyMatrixFixed
{
    bool matrix[Size][Size] { };
    int in_degree[Size] { };
    int out_degree[Size] { };

    constexpr static std::size_t SIZE = Size;

    static constexpr std::size_t size()
    {
        return Size;
    }

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

    constexpr bool is_root(const int v) const
    {
        return in_degree[v] == 0;
    }

    constexpr bool has_edge(const int from, const int to) const
    {
        return matrix[from][to] != 0;
    }

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
