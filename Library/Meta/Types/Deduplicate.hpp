#pragma once

#include <type_traits>
#include <utility>
#include <bitset>

#include <Usagi/Library/Meta/Types/Containers/List/TypeList.hpp>

namespace usagi::meta
{
/**
 * Calculate the uniqueness of provided type list. The first appearance of a
 * type in the list is regarded as unique and the bit with the same index in
 * the returned bitset is set to 1.
 * todo: return an actual bitset. ullong only supports 64 types.
 * @tparam Ts List of types.
 * @return a bitset where each bit corresponds the uniqueness of a type.
 */
template <typename... Ts>
consteval auto tag_unique_types()
{
    constexpr std::size_t num_types = sizeof...(Ts);
    constexpr auto index_seq = std::make_index_sequence<num_types> { };
    std::bitset<num_types> unique_types;
    // std::uint64_t unique_types = 0;
    // PrevIndices... should be [0, index)
    auto op_is_duplicated =
        []<typename CurT, typename Int, Int... PrevIndices>(
            // const std::size_t index,
            std::integer_sequence<Int, PrevIndices...> idx_seq) -> bool
    {
        // static_assert(sizeof...(PrevIndices) < index);
        return std::disjunction_v<std::is_same<CurT, Ts...[PrevIndices]>...>;
    };
    auto op_visit_types = [&]<typename Int, Int... AllIndices>(
        std::integer_sequence<Int, AllIndices...> idx_seq) -> void
    {
        ((unique_types[AllIndices] = !op_is_duplicated
            .template operator()<Ts...[AllIndices]>(
                // make_index_sequence automatically creates a
                // sequence ending with AllIndices - 1.
                std::make_index_sequence<AllIndices>()
            )
        ), ...);
    };
    op_visit_types(index_seq);
    return unique_types.to_ullong();
}

static_assert(tag_unique_types<int, int, double, char>() == 0b1101);
    // std::bitset<4>{ "1101" });

template <template <typename...> typename List, typename... Ts, typename... Us>
consteval auto concatenate_list(List<Ts...>, List<Us...>)
{
    return List<Ts..., Us...>();
}

template <
    template <typename...> typename List,
    typename... Ts
>
consteval auto deduplicate_type_list()
{
    if constexpr(sizeof...(Ts) == 0)
    {
        return []<typename T = void>() { return List<>(); };
    }
    else
    {
        auto op = []<std::uint64_t UniqueFlags = tag_unique_types<Ts...>()>()
        {
            if constexpr(sizeof...(Ts) == 0)
            {
                return List<>();
            }
            else
            {
                auto op_reduce = []<typename U, typename... Us>()
                {
                    // constexpr std::bitset<sizeof...(Us)> flags {
                        // unique_flags.to_ullong() >> 1
                        // UniqueFlags >> 1
                    // };
                    // drop the prev type
                    if constexpr(sizeof...(Us) == 0)
                    {
                        return List<>();
                    }
                    else
                    {
                        return deduplicate_type_list<List, Us...>()
                            .template operator()<(UniqueFlags >> 1)>();
                    }
                };
                if constexpr((UniqueFlags & 0b1) == true)
                {
                    return concatenate_list(
                        List<Ts...[0]>(), op_reduce.template operator()<Ts...>()
                    );
                }
                else
                {
                    return op_reduce.template operator()<Ts...>();
                }
            }
        };
        return op;
    }
}

static_assert(deduplicate_type_list<TypeList>()() ==
    TypeList<>());
static_assert(deduplicate_type_list<TypeList, int>()() ==
    TypeList<int>());
static_assert(deduplicate_type_list<TypeList, int, int, bool>()() ==
    TypeList<int, bool>());
static_assert(deduplicate_type_list<TypeList, int, char, int, bool>()() ==
    TypeList<int, char, bool>());
}
