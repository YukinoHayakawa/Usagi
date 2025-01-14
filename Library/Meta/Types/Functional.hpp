#pragma once

#include <utility>

#include <Usagi/Library/Meta/Functional.hpp>
#include <Usagi/Library/Meta/Concepts/Functional.hpp>

namespace usagi::meta::types
{
/**
 * Invoke the provided operator with each type in Ts... together with indices.
 * Nop if `sizeof(Ts...) == 0`.
 * @tparam Ts Parameter pack of types.
 * @param op A generic invocable that accepts `<Index, Type>`.
 */
template <typename... Ts>
consteval void map(auto &&op) requires
    (concepts::GenericIndexedTypenameOperator<decltype(op), Ts> && ...)
{
    auto do_map = [&]<typename Int, Int... Indices>(
        std::integer_sequence<Int, Indices...>)
    {
        // you don't want the compiler to yell at you empty pack cannot be
        // indexed, right?
        if constexpr(sizeof...(Ts) > 0)
        {
            // both the
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
 * types to keep.
 * @tparam Ts List of types.
 * @param tail An empty type list when there is nothing left to deal with.
 * @param op_accumulate An invocable constexpr object that concatenates the head
 * with the tail list when the head is to be kept.
 * @param op_cond_keep_cur_t An invocable constexpr object deciding which type
 * should be kept by index.
 * @return A type list with only elements decided to be kept by
 * op_cond_keep_cur_t.
 */
template <typename... Ts>
consteval auto reduce(
    auto &&tail,
    auto &&op_accumulate,
    auto &&op_cond_keep_cur_t) requires
    concepts::IndexedBooleanOperator<decltype(op_cond_keep_cur_t)>
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
                /*
                 * Lambdas are constexpr by default whenever possible (see
                 * https://en.cppreference.com/w/cpp/language/lambda#:~:text=a%20constexpr%20function.-,If%20operator()%20satisfy%20all,(since%20C%2B%2B20),-Specifies%20that%20operator).
                 * However, op_cond_keep_cur_t is not because it is a parameter.
                 * But since we can know its unique type, a constexpr instance
                 * can be created for compile time evaluation.
                 */
                construct_constexpr_invocable<decltype(op_cond_keep_cur_t)>()
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
