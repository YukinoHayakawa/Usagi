#pragma once

#include <Usagi/Executor/TaskGraph.hpp>
#include <Usagi/Library/Meta/CompileTimeError.hpp>
#include <Usagi/Library/Meta/Stack.hpp>
#include <Usagi/Library/Graph/TopologicalSort.hpp>

namespace usagi
{
enum class ErrorCode
{
    SUCCEED,
    // A write system is not on the shortest path between begin and end
    CPG_SHORTCUT_WRITE_SYSTEM,
};

using TaskGraphError = CompileTimeError<
    ErrorCode,
    int, // vertex index
    ErrorCode::SUCCEED
>;

using TaskGraphErrorCode = TaskGraphError::ErrorCode;

template <ErrorCode Code, int Vertex>
using TaskGraphErrorCodeCheck = TaskGraphError::CheckErrorCode<Code, Vertex>;

template <std::size_t Size>
constexpr bool sat_no_write_systems(const SystemAccessTraits<Size> &sat)
{
    for(auto i = 0; i < Size; ++i)
    {
        if(sat[i] == ComponentAccess::WRITE)
            return false;
    }
    return true;
}

template <int Size, std::size_t Ss = Size>
constexpr TaskGraphErrorCode cpg_validate(
    const GraphAdjacencyMatrix<Size> &cpg,
    const SystemAccessTraits<Ss> &sat
)
{
    // The cpg is validated as per the descriptions in:
    // https://yuki.moe/blog/index.php/2020/03/30/concurrent-entity-access/
    // Note that the cpg must be transitive reduced.

    // If all Systems are Read Systems, the dependencies among them are ignored.

    if(sat_no_write_systems(sat))
        return TaskGraphErrorCode(ErrorCode::SUCCEED);

    // If there exists at least one Write System, the weight of the shortest
    // path from Begin System to End System equals the number of Write Systems,
    // assuming all Write Systems have a weight of 1 and all Read Systems have a
    // weight of 0. In simple words, in all topological orders of the cpg, the
    // Write System are ordered in the same way and a Read System always sits
    // between the same pair of Write Systems (assuming the dummy Begin and End
    // Systems are Write Systems, too), so it does not incur a race condition.

    // Ref: https://www2.seas.gwu.edu/~simhaweb/alg/modules/module9/module9.html

    // the last two are dummy begin & end systems
    constexpr int Begin = Size;
    constexpr int End = Size + 1;
    std::array<unsigned, Size + 2> priority { };
    for(auto i = 0; i < Size + 2; ++i) priority[i] = -1;
    std::array<int, Size + 2> predecessor { };
    for(auto i = 0; i < Size + 2; ++i) predecessor[i] = -1;

    auto order = topological_sort(cpg);
    meta::Stack<Size> write_systems;

    auto search = [
        &priority, &predecessor, &cpg, &sat, Begin, End
    ](const int v)
    {
        int w = 0;

        auto update = [&priority, &predecessor, &w](const int v, const int u)
        {
            // found a better route to u via v
            // because read systems have a weight of 0, we have to use >=
            // rather than >.
            if(priority[u] >= priority[v] + w)
            {
                priority[u] = priority[v] + w;
                predecessor[u] = v;
            }
        };

        // the dummy begin system precedes all systems with a 0 in-degree
        if(v == Begin)
        {
            w = 0;
            for(auto i = 0; i < Size; ++i)
            {
                // link begin system to all systems with 0 in-degree
                if(cpg.in_degree[i] > 0 ||
                    sat[i] == ComponentAccess::NONE)
                    continue;
                update(v, i);
            }
        }
        else
        {
            // this is expected to be either write or read, never none.
            w = sat[v] == ComponentAccess::WRITE ? 1 : 0;
            // we have met a endpoint system, link it with the end system
            if(cpg.out_degree[v] == 0)
            {
                update(v, End);
            }
            else
            {
                // visit children of v
                for(auto i = 0; i < Size; ++i)
                {
                    // not child of v.
                    if(cpg.matrix[v][i] == false)
                        continue;
                    update(v, i);
                }
            }
        }
    };

    search(Begin);
    while(!order.empty())
    {
        const auto s = order.pop();
        if(sat[s] == ComponentAccess::NONE)
            continue;
        // store write systems in reserve order
        if(sat[s] == ComponentAccess::WRITE)
            write_systems.push(s);
        search(s);
    }

    auto cur = End;
    while(cur != -1)
    {
        const auto p = predecessor[cur];
        if(p == Begin) break;
        if(sat[p] == ComponentAccess::WRITE)
        {
            const auto w = write_systems.pop();
            // an unmatch in the write system order. could be a write system
            // got short-cut or there are multiple possible write paths.
            if(w != p)
            {
                return TaskGraphErrorCode(
                    ErrorCode::CPG_SHORTCUT_WRITE_SYSTEM,
                    w
                );
            }
        }
        cur = p;
    }

    return TaskGraphErrorCode(ErrorCode::SUCCEED);
}
}
