﻿#pragma once

#include <Usagi/Entity/EntityDatabase.hpp>
#include <Usagi/Entity/System.hpp>
#include <Usagi/Entity/detail/ComponentAccessSystemAttribute.hpp>
#include <Usagi/Library/Graph/AdjacencyMatrixFixed.hpp>
#include <Usagi/Library/Graph/TopologicalSort.hpp>
#include <Usagi/Library/Meta/Tuple.hpp>

#include "detail/SystemAccessTraits.hpp"

namespace usagi
{
template <System... Sys>
class TaskGraph
{
public:
    constexpr static std::size_t NUM_SYSTEMS = sizeof...(Sys);
    using SystemListT = std::tuple<Sys...>;
    using GraphT = graph::AdjacencyMatrixFixed<NUM_SYSTEMS>;
    using ComponentUsageT = SystemComponentUsage<Sys...>;
    using EntityDatabaseT = EntityDatabase<ComponentUsageT>;

private:
    GraphT mSystemPrecedenceGraph;

public:
    template <System From, System To>
    constexpr void precede()
    {
        static_assert(has_type_v<From, SystemListT>);
        static_assert(has_type_v<To, SystemListT>);

        mSystemPrecedenceGraph.add_edge(
            INDEX<From, SystemListT>,
            INDEX<To, SystemListT>
        );
    }

    template <Component C>
    constexpr auto system_access_traits() const
    {
        return sat<C>(std::make_index_sequence<NUM_SYSTEMS>());
    }

    template <Component C>
    constexpr GraphT component_precedence_graph() const
    {
        GraphT cpg;
        // visit all possible vertices
        cpg_helper_row<C>(cpg, std::make_index_sequence<NUM_SYSTEMS>());
        cpg.transitive_reduce();
        return cpg;
    }

    template <Component C>
    constexpr GraphT transformed_component_precedence_graph() const
    {
        auto cpg = component_precedence_graph<C>();
        // constexpr auto sat = system_access_traits<C>();
        // constexpr auto tps = topological_sort(cpg);
        //
        // GraphT tcpg;
        // int prev_sys = -1, next_sys = -1;
        // while(!tps.empty())
        // {
        //
        // }
        // TODO impl transformation

        return cpg;
    }

    constexpr GraphT reduced_task_graph() const
    {
        GraphT rtg;
        rtg_helper(rtg, ComponentUsageT());
        rtg.transitive_reduce();
        return rtg;
    }

private:
    template <Component C>
    constexpr void rtg_helper_c(GraphT &rtg) const
    {
        // calculate cpg & merge results into the base task graph
        rtg |= transformed_component_precedence_graph<C>();
    }

    template <Component... Cs>
    constexpr void rtg_helper(GraphT &rtg, ComponentFilter<Cs...>) const
    {
        // for each type of component
        (..., rtg_helper_c<Cs>(rtg));
    }

    template <Component C, std::size_t From, std::size_t To>
    constexpr void cpg_helper_edge(GraphT &cpg) const
    {
        using SystemFrom = std::tuple_element_t<From, SystemListT>;
        using SystemTo = std::tuple_element_t<To, SystemListT>;

        constexpr bool sys_from = SystemCanAccessComponent<SystemFrom, C>;
        constexpr bool sys_to = SystemCanAccessComponent<SystemTo, C>;

        // If both systems access the same component, the edge is preserved
        if(sys_from && sys_to && mSystemPrecedenceGraph.has_edge(From, To))
        {
            cpg.add_edge(From, To);
        }
    }

    template <Component C, std::size_t I, std::size_t... CIs>
    constexpr void cpg_helper_children(
        GraphT &cpg,
        std::index_sequence<CIs...>) const
    {
        // visit each child of I
        (..., cpg_helper_edge<C, I, CIs>(cpg));
    }

    template <Component C, std::size_t I>
    constexpr void cpg_helper_vertex(GraphT &cpg) const
    {
        cpg_helper_children<C, I>(cpg, std::make_index_sequence<NUM_SYSTEMS>());
    }

    template <Component C, std::size_t... Is>
    constexpr void cpg_helper_row(GraphT &cpg, std::index_sequence<Is...>) const
    {
        // visit every system
        (..., cpg_helper_vertex<C, Is>(cpg));
    }

    template <Component C, std::size_t... Is>
    constexpr auto sat(std::index_sequence<Is...>) const
    {
        SystemAccessTraits<NUM_SYSTEMS> traits { };
        // visit every system
        (..., (traits[Is] = SystemHighestComponentAccess<
            std::tuple_element_t<Is, SystemListT>,
            C
        >));
        return traits;
    }
};
}
