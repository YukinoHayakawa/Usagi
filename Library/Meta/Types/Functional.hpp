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

/**
 * Process a list of types with certain constexpr conditions to decide which
 * types to save.
 * @tparam Ts List of types.
 * @param tail An empty list.
 * @param op_accumulate An invocable constexpr object that handles types to be
 * saved.
 * @param op_cond_keep_cur_t An invocable constexpr object deciding which type
 * should be kept.
 * @return A type list with only elements decided to be kept by
 * op_cond_keep_cur_t.
 */
template <typename... Ts>
consteval auto reduce(
    auto &&tail,
    auto &&op_accumulate,
    auto &&op_cond_keep_cur_t) requires requires
{
    // the op must evaluate to bool
    std::is_same_v<
        decltype(op_cond_keep_cur_t.template operator()<sizeof...(Ts)>()),
        bool
    > == true;
}
{
    /*
     * sort of the base case
     */
    if constexpr(sizeof...(Ts) == 0)
    {
        return std::forward<decltype(tail)>(tail);
    }
    else
    {
        /*
         * somehow calling reduce<> from inside the lambda doesn't work.
         * I don't know whether it is a compiler bug or what because I was
         * calling like reduce<OtherTs..., CurTIndex + 1>(...), providing all
         * the template arguments. however, neither clang nor gcc compiles that.
         * so it seems we can only recurse with the lambda.
         * note that a lambda capturing unknown parameters cannot be constexpr.
         * - update: ok it's not a compiler bug. it is decided by the standard
         *   that there shouldn't be any template parameter after a parameter
         *   pack, unless it can be deducted. in other words, that parameter
         *   cannot be specified.
         *   see https://stackoverflow.com/a/53790931/2150446
         */
        auto op_reduce = [&]<
            // the index of the current type in Ts...
            std::size_t CurTIndex,
            // the current type
            typename CurT,
            // remaining types
            typename... OtherTs,
            // this must be constexpr because it is used for determining
            // the control path.
            bool KeepCurT =
                // this doesn't work. op_cond_keep_cur_t is not constexpr:
                // op_cond_keep_cur_t.template operator()<CurTIndex>()
                // so we have to use something like this to put it in an
                // unevaluated context.
                std::remove_cvref_t<decltype(op_cond_keep_cur_t)>()
                    .template operator()<CurTIndex>() == true
        >(auto &&self) consteval
        {
            // base case. creates something like List<CurT>.
            if constexpr(sizeof...(OtherTs) == 0)
            {
                if constexpr(KeepCurT)
                {
                    return op_accumulate.template operator()<CurTIndex, CurT>(
                        std::forward<decltype(tail)>(tail)
                    );
                }
                else
                {
                    return std::forward<decltype(tail)>(tail);
                }
            }
            else
            {
                auto recursive_tail =
                    self.template operator()<CurTIndex + 1, OtherTs...>(self);
                // if the current type is to be kept, accumulate it.
                // how the type list is accumulated is up to the user.
                if constexpr(KeepCurT)
                {
                    return op_accumulate.template operator()<CurTIndex, CurT>(
                        recursive_tail
                    );
                }
                // drop CurT and proceed to remaining types.
                else
                {
                    return recursive_tail;
                }
            }
        };
        return op_reduce.template operator()<0, Ts...>(op_reduce);
    }
}
}
