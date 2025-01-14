#pragma once

#include <Usagi/Library/Meta/Types/TypeTag.hpp>

namespace usagi::meta::types
{
/*
 * Just a container carrying a list of typenames as a parameter pack. Can be
 * rebound to other templates.
 */
template <typename... Ts>
struct TypeList
{
    template <template <typename...> typename T>
    using RebindT = T<Ts...>;
};

template <typename... Ts>
struct AggregatedBaseClassList : Ts...
{
    template <template <typename...> typename T>
    using RebindT = T<Ts...>;

    template <typename... Us>
    using ConcatenatedT = AggregatedBaseClassList<Ts..., Us...>;

    template <typename U>
    static constexpr bool IsDuplicated =
        std::is_base_of_v<U, AggregatedBaseClassList>;

    template <typename U>
    using UniqueConcatenatedT = std::conditional_t<
        IsDuplicated<U>,
        AggregatedBaseClassList,
        AggregatedBaseClassList<Ts..., U>
    >;
};

static_assert(std::is_same_v<
    AggregatedBaseClassList<>::UniqueConcatenatedT<TypeTag<float>>,
    AggregatedBaseClassList<TypeTag<float>>
>);

static_assert(std::is_same_v<
    AggregatedBaseClassList<TypeTag<int>>::UniqueConcatenatedT<TypeTag<float>>,
    AggregatedBaseClassList<TypeTag<int>, TypeTag<float>>
>);

static_assert(std::is_same_v<
    AggregatedBaseClassList<TypeTag<int>>::UniqueConcatenatedT<TypeTag<int>>,
    AggregatedBaseClassList<TypeTag<int>>
>);

template <typename... Ts>
struct AggregatedUniqueBaseClassList
{
    static consteval auto eval_class_list(
        auto &&with /*= []<std::size_t, typename, bool>() { }*/)
    {
        auto op = [&]<std::size_t CurIndex, typename... Args>(
            AggregatedBaseClassList<Args...> prev,
            auto &&self)
        {
            if constexpr (CurIndex == sizeof...(Ts))
            {
                return prev;
            }
            else
            {
                with.template operator()<
                    CurIndex,
                    Ts...[CurIndex],
                    !AggregatedBaseClassList<Args...>::
                        template IsDuplicated<Ts...[CurIndex]>
                >();
                return self.template operator()<CurIndex + 1>(
                    typename AggregatedBaseClassList<Args...>::
                        template UniqueConcatenatedT<Ts...[CurIndex]>(),
                    self
                );
            }
        };
        return op.template operator()<0>(AggregatedBaseClassList(), op);
    }

    template <typename T>
    static consteval bool has_base_class()
    {
        return std::is_base_of_v<T, decltype(eval_class_list())>;
    }
};
/*
namespace details
{
template <std::size_t CurIndex, typename... Ts>
struct AggregatedIndexedBaseClassList_CarryIndex;

template <std::size_t CurIndex>
struct AggregatedIndexedBaseClassList_CarryIndex<CurIndex>
{
};

template <std::size_t CurIndex, typename T>
struct AggregatedIndexedBaseClassList_CarryIndex<CurIndex, T>
    : IndexedTypeTag<CurIndex, T>
{
};

template <std::size_t CurIndex, typename T, typename... Ts>
struct AggregatedIndexedBaseClassList_CarryIndex<CurIndex, T, Ts...>
    : IndexedTypeTag<CurIndex, T>
    , AggregatedIndexedBaseClassList_CarryIndex<CurIndex + 1, Ts...>
{
};
}
*/

//
// template <typename T>
// struct IsTypeInAggregatedIndexedBaseClassList
// {
//     static constexpr bool Value = false;
// };
//
// template <std::size_t Index, typename T>
// struct IsTypeInAggregatedIndexedBaseClassList<IndexedTypeTag<Index, T>>
// {
//     static constexpr bool Value = true;
// };

// template <typename T>
// concept ConvertibleToIndexedTypeTag = requires(T t)
// {
//     []<std::size_t Index, template U>()
// }

/*
template <typename Prev, bool CurIsDuplicated, typename... Ts>
struct AggregatedUniqueBaseClassList_Helper;

template <typename Prev, bool CurIsDuplicated>
struct AggregatedUniqueBaseClassList_Helper<Prev, CurIsDuplicated>
{
};

template <typename Prev, typename T>
struct AggregatedUniqueBaseClassList_Helper<Prev, false, T>
    : TypeTag<T>
{
};

template <typename Prev, typename T>
struct AggregatedUniqueBaseClassList_Helper<Prev, true, T>
{
};

template <typename Prev, typename T, typename... Ts>
struct AggregatedUniqueBaseClassList_Helper<Prev, false, T, Ts...>
    : TypeTag<T>
    , AggregatedUniqueBaseClassList_Helper<
        typename Prev::template ConcatenatedT<TypeTag<T>>,
        std::is_base_of_v<TypeTag<Ts...[0]>, typename Prev::template ConcatenatedT<T>>,
        Ts...
    >
{
};

template <typename Prev, typename T, typename... Ts>
struct AggregatedUniqueBaseClassList_Helper<Prev, true, T, Ts...>
    : AggregatedUniqueBaseClassList_Helper<
        Prev,
        std::is_base_of_v<TypeTag<Ts...[0]>, Prev>,
        Ts...
    >
{
};
}

template <typename... Ts>
struct AggregatedUniqueBaseClassList
    : details::AggregatedUniqueBaseClassList_Helper<
        AggregatedBaseClassList<>,
        false,
        Ts...
    >
{
};
*/

/*
template <typename... Ts>
struct AggregatedUniqueBaseClassList;

template <typename... Ts>
struct AggregatedUniqueBaseClassList;

template <typename T>
consteval bool is_type_in_list(auto &&type_tag)
{
    return details::IsTypeInAggregatedIndexedBaseClassList<
        decltype(type_tag)
    >::Value;
}
*/

/*
template <typename T, std::size_t Index>
consteval bool is_type_in_list(IndexedTypeTag<Index, T>)
{
    return true;
}
*/

/*
template <typename... Ts>
struct AggregatedBaseClassList : Ts...
{
    template <template <typename...> typename T>
    using RebindT = T<Ts...>;
};

template <typename... Ts>
struct AggregatedUniqueBaseClassList;

template <>
struct AggregatedUniqueBaseClassList<>
{
    using UniqueTypeListT = AggregatedBaseClassList<>;
};

template <typename T>
struct AggregatedUniqueBaseClassList<T>
{
    using UniqueTypeListT = AggregatedBaseClassList<T>;
};

template <typename T, typename... Ts>
struct AggregatedBaseClassList<T, Ts...>
{
    using UniqueTypeListT = std::conditional_t<
        std::is_base_of_v<
            TypeTag<T>,
            typename AggregatedBaseClassList<Ts...>::UniqueTypeListT
        >,

    >
};
*/

// static_assert(std::is_base_of_v<tag<int>, AggregatedBaseClassList<int>>);
// static_assert(std::is_base_of_v<tag<int>, AggregatedBaseClassList<int, int>>);
/*
template <>
struct AggregatedBaseClassList<>
{
    using UniqueT = AggregatedBaseClassList<>;
};

template <typename T>
struct AggregatedBaseClassList<T> : T
{
    using UniqueT = AggregatedBaseClassList<T>;
};

template <typename T, typename... Ts>

struct AggregatedBaseClassList<T, Ts...>  : T

{

};
*/
}
