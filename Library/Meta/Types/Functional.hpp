#pragma once

#include <utility>

namespace usagi::meta::types
{
/**
 * Map given operator to every type in Ts...
 * @tparam Ts List of types.
 * @param op Operator to apply.
 */
template <typename... Ts>
consteval void map(auto &&op)
{
    auto do_map = [&]<typename Int, Int... Indices>(
        std::integer_sequence<Int, Indices...>)
    {
        // you don't want the compiler to yell at you empty pack cannot be
        // indexed, right?
        if constexpr(sizeof...(Ts) > 0)
        {
            ((op.template operator()<Indices, Ts...[Indices]>()), ...);
        }
    };
    /*
     * Note that the integer_sequence cannot be put as an argument of the map()
     * function itself. If you do so, you will have to invoke map() with a
     * make_index_sequence every time, which is undesirable. Assigning some
     * default value to the argument won't work, as template arguments cannot
     * be deduced from default parameters. You will encounter errors like
     * Int cannot be deduced, etc. Therefore, we handle it inside this function.
     */
    do_map(std::make_index_sequence<sizeof...(Ts)>());
}
}
