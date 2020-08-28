#pragma once

#include <algorithm>
#include <iterator>

#include "Graph.hpp"

namespace usagi::graph
{
template <
    typename G,
    typename Traits = typename DefaultGraphTrait<G>::TraitT
>
class PathReverseIterator
{
    using VertexPrevArrayT = typename Traits::template VertexAttributeArray<
        typename Traits::VertexIndexT
    >;
    using VertexIndexT = typename Traits::VertexIndexT;

    const VertexPrevArrayT *prev = nullptr;
    VertexIndexT i = Traits::INVALID_VERTEX_ID;
    VertexIndexT end = Traits::INVALID_VERTEX_ID;

public:
    using difference_type = int;
    using value_type = int;
    using pointer = void;
    using reference = int;
    using iterator_category = std::input_iterator_tag;

    constexpr PathReverseIterator() = default;

    explicit constexpr PathReverseIterator(
        const VertexPrevArrayT &prev,
        VertexIndexT i = Traits::INVALID_VERTEX_ID,
        VertexIndexT end = Traits::INVALID_VERTEX_ID)
        : prev(&prev)
        , i(i)
        , end(end)
    {
    }

    constexpr decltype(auto) operator++()
    {
        if(i == end) i = Traits::INVALID_VERTEX_ID;
        else i = (*prev)[i];
        return *this;
    }

    constexpr auto operator++(int)
    {
        auto old = *this;
        ++(*this);
        return old;
    }

    constexpr auto operator*() const
    {
        return i;
    }

    constexpr bool operator==(const PathReverseIterator &rhs) const
    {
        return i == rhs.i;
    }
};

template <
    typename G,
    typename Traits = typename DefaultGraphTrait<G>::TraitT
>
constexpr auto path_length(
    const typename Traits::template VertexAttributeArray<
        typename Traits::VertexIndexT> &prev,
    const typename Traits::VertexIndexT src,
    const typename Traits::VertexIndexT dest)
    requires Graph<G, Traits>
{
    return std::distance(
        PathReverseIterator<G, Traits>(prev, dest, src),
        PathReverseIterator<G, Traits>(prev)
    );
}

template <
    typename G,
    typename Traits = typename DefaultGraphTrait<G>::TraitT
>
constexpr auto path_to_array(
    const typename Traits::template VertexAttributeArray<
        typename Traits::VertexIndexT> &prev,
    const typename Traits::VertexIndexT src,
    const typename Traits::VertexIndexT dest,
    Traits t = { })
    requires DirectedGraph<G, Traits>
{
    auto length = path_length<G, Traits>(prev, src, dest);
    typename Traits::template VertexAttributeArray<
        typename Traits::VertexIndexT> array;
    t.resize(array, length);

    auto cur = PathReverseIterator<G, Traits>(prev, dest, src);
    auto end = PathReverseIterator<G, Traits>(prev);
    // use the array like a stack by writing from the back
    std::copy(cur, end, array.rbegin());

    return array;
}
}
