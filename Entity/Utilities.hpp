#pragma once

#include <Usagi/Library/Meta/ParameterPack/Deduplicate.hpp>
#include <Usagi/Library/Meta/ParameterPack/CollectInnerLists.hpp>

namespace usagi
{
template <typename T>
struct DeclaredComponentReadAccess
{
    using InnerListT = C<>;
};

template <StructDeclaresReadAccess T>
struct DeclaredComponentReadAccess<T>
{
    using InnerListT = typename T::ReadAccess;
};

template <typename T>
struct DeclaredComponentWriteAccess
{
    using InnerListT = C<>;
};

template <StructDeclaresWriteAccess T>
struct DeclaredComponentWriteAccess<T>
{
    using InnerListT = typename T::WriteAccess;
};

/*
template <typename T>
using DeclaredComponentReadAccess = typename T::ReadAccess;

template <typename T>
using DeclaredComponentWriteAccess = typename T::WriteAccess;
*/

template <typename... Ts>
using CollectReadAccessUnique = 
    meta::Deduplicated<
        meta::CollectedInnerList<DeclaredComponentReadAccess, Ts...>
    >;

template <typename... Ts>
using CollectWriteAccessUnique = 
    meta::Deduplicated<
        meta::CollectedInnerList<DeclaredComponentWriteAccess, Ts...>
    >;

/*
struct A
{
    using ReadAccess = C<int, bool>;
};

struct B
{
    using ReadAccess = C<bool, double>;
};

struct D
{
};

static_assert(std::is_same_v<
    CollectReadAccessUnique<A, B, D>,
    C<int, bool, double>
>);
*/

}
