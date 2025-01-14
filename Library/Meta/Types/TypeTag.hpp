#pragma once

namespace usagi::meta::types
{
template <typename T>
struct TypeTag
{
    using TypeT = T;
};

template <std::size_t Index, typename T>
struct IndexedTypeTag
{
    static constexpr auto IndexVal = Index;
    using TypeT = T;
};
}
