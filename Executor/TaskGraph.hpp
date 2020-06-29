#pragma once

#include <Usagi/Entity/System.hpp>
#include <Usagi/Entity/detail/ComponentAccessSystemAttribute.hpp>
#include <Usagi/Library/Graph/GraphAdjacencyMatrix.hpp>
#include <Usagi/Library/Meta/Tuple.hpp>

namespace usagi
{
template <int Size>
struct SystemPrecedenceGraph : GraphAdjacencyMatrix<Size>
{
    ComponentAccess system_traits[Size] { };

    constexpr bool operator==(const SystemPrecedenceGraph &rhs) const
    {
        if(!GraphAdjacencyMatrix<Size>::equal(rhs))
            return false;

        for(auto i = 0; i < Size; ++i)
            if(this->system_traits[i] != rhs.system_traits[i])
                return false;

        return true;
    }
};

template <System... Sys>
class TaskGraph : SystemPrecedenceGraph<sizeof...(Sys)>
{
public:
    constexpr static std::size_t NUM_SYSTEMS = sizeof...(Sys);
    using SystemList = std::tuple<Sys...>;
    using Graph = SystemPrecedenceGraph<NUM_SYSTEMS>;

    template <System From, System To>
    constexpr void precede()
    {
        static_assert(has_type_v<From, SystemList>);
        static_assert(has_type_v<To, SystemList>);

        this->add_edge(
            INDEX<From, SystemList>,
            INDEX<To, SystemList>
        );
    }

    template <Component C>
    constexpr Graph component_precedence_graph() const
    {
        Graph cpg;
        // visit all possible vertices
        cpg_helper_row<C>(cpg, std::make_index_sequence<NUM_SYSTEMS>());
        cpg.transitive_reduce();
        return cpg;
    }

    /*
    constexpr Graph reduced_task_graph() const
    {
    }
    */

private:
    template <Component C, std::size_t From, std::size_t To>
    constexpr void cpg_helper_edge(Graph &cpg) const
    {
        using SystemFrom = std::tuple_element_t<From, SystemList>;
        using SystemTo = std::tuple_element_t<To, SystemList>;

        constexpr bool sys_from = SystemCanAccessComponent<SystemFrom, C>;
        constexpr bool sys_to = SystemCanAccessComponent<SystemTo, C>;

        // If both systems access the same component, the edge is preserved
        if(sys_from && sys_to && this->has_edge(From, To))
        {
            cpg.add_edge(From, To);
        }
    }

    template <Component C, std::size_t I, std::size_t... CIs>
    constexpr void cpg_helper_children(
        Graph &cpg,
        std::index_sequence<CIs...>) const
    {
        // visit each child of I
        (..., cpg_helper_edge<C, I, CIs>(cpg));
    }

    template <Component C, std::size_t I>
    constexpr void cpg_helper_vertex(Graph &cpg) const
    {
        // visit children of system I
        cpg.system_traits[I] = SystemHighestComponentAccess<
            std::tuple_element_t<I, SystemList>,
            C
        >;
        cpg_helper_children<C, I>(cpg, std::make_index_sequence<NUM_SYSTEMS>());
    }

    template <Component C, std::size_t... Is>
    constexpr void cpg_helper_row(Graph &cpg, std::index_sequence<Is...>) const
    {
        // visit every system
        (..., cpg_helper_vertex<C, Is>(cpg));
    }
};
}
