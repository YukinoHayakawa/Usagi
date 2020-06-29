#pragma once

#include <array>

namespace usagi
{
template <int Size>
struct GraphAdjacencyMatrix
{
    bool matrix[Size][Size] { };
    int in_degree[Size] { };
    int out_degree[Size] { };

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

    constexpr bool equal(const GraphAdjacencyMatrix &rhs) const
    {
        for(auto i = 0; i < Size; ++i)
            for(auto j = 0; j < Size; ++j)
                if(matrix[i][j] != rhs.matrix[i][j])
                    return false;

        return true;
    }

    constexpr bool operator==(const GraphAdjacencyMatrix &rhs) const
    {
        return equal(rhs);
    }

    constexpr bool operator!=(const GraphAdjacencyMatrix &rhs) const
    {
        return !equal(rhs);
    }

    using Flag = std::array<bool, Size>;

    // Ref: https://www.geeksforgeeks.org/detect-cycle-in-a-graph/
    constexpr bool is_cyclic_helper(int v, Flag &visited, Flag &visiting) const
    {
        if(!visited[v])
        {
            visited[v] = true;
            // if this vertex is not visited, it cannot be on the recursive
            // stack
            visiting[v] = true;

            // visit children of v
            for(auto i = 0; i < Size; ++i)
            {
                // not connected
                if(matrix[v][i] == false)
                    continue;
                // new child, dive in
                if(!visited[i])
                {
                    if(is_cyclic_helper(i, visited, visiting))
                        return true;
                }
                    // getting back to one of the ancestor -> a cycle was found
                else if(visiting[i])
                    return true;
            }

            visiting[v] = false;
        }
        return false;
    }

    constexpr bool is_cyclic() const
    {
        std::array<bool, Size> visited { };
        std::array<bool, Size> visiting { };

        for(auto i = 0; i < Size; ++i)
        {
            if(is_cyclic_helper(i, visited, visiting))
                return true;
        }
        return false;
    }

    constexpr void transitive_reduce_helper(const int v, const int child)
    {
        // visit indirect descendents of v through child
        for(auto i = 0; i < Size; ++i)
        {
            if(matrix[child][i] == false)
                continue;

            // otherwise, there is an indirect path from v to i, remove the
            // direct path from v to i.
            remove_edge(v, i);

            transitive_reduce_helper(v, i);
        }
    }

    // Ref: https://cs.stackexchange.com/a/29133
    constexpr void transitive_reduce()
    {
        // perform DFS on all vertices, assuming the graph is a DAG
        for(auto v = 0; v < Size; ++v)
        {
            // visit children of v
            for(auto i = 0; i < Size; ++i)
            {
                // not child of v
                if(matrix[v][i] == false)
                    continue;

                transitive_reduce_helper(v, i);
            }
        }
    }
};
}
