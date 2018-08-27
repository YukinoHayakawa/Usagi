#pragma once

#include <cstddef>
#include <array>
#include <algorithm>

#include <Carrot/Preprocessor/unpack.hpp>

namespace usagi
{
template <
    typename SrcEnum,
    typename DestEnum
>
struct EnumTranslationTraits;

template <
    typename SrcEnum,
    typename DestEnum
>
struct EnumTranslator
{
    // todo c++20 constexpr std::find
    static /*constexpr*/ DestEnum translate(const SrcEnum e)
    {
        using Traits = EnumTranslationTraits<SrcEnum, DestEnum>;
        auto i = std::find(Traits::src.begin(), Traits::src.end(), e);
        if(i != Traits::src.end())
            return Traits::dest[i - Traits::src.begin()];
        throw std::runtime_error("Could not translate the enum.");
    }
};
}

#define USAGI_ENUM_TRANSLATION_DECL(src_t, dst_t) \
/*constexpr*/ dst_t translate(const src_t e); \
/*constexpr*/ src_t translate(const dst_t e) \
/**/

#define USAGI_ENUM_TRANSLATION_BASE(ns, src_t, dst_t, num_elem, src_elems, dst_elems) \
template <> \
struct ::usagi::EnumTranslationTraits<src_t, dst_t> \
{ \
    static constexpr std::array<src_t, num_elem> src { USAGI_UNPACK src_elems }; \
    static constexpr std::array<dst_t, num_elem> dest { USAGI_UNPACK dst_elems }; \
}; \
template <> \
struct ::usagi::EnumTranslationTraits<dst_t, src_t> \
{ \
    static constexpr std::array<dst_t, num_elem> src { USAGI_UNPACK dst_elems }; \
    static constexpr std::array<src_t, num_elem> dest { USAGI_UNPACK src_elems }; \
}; \
/*constexpr*/ dst_t ns translate(const src_t e) \
{ \
    return ::usagi::EnumTranslator<src_t, dst_t>::translate(e); \
} \
/*constexpr*/ src_t ns translate(const dst_t e) \
{ \
    return ::usagi::EnumTranslator<dst_t, src_t>::translate(e); \
} \
/**/

#define USAGI_ENUM_TRANSLATION(src_t, dst_t, num_elem, src_elems, dst_elems) \
USAGI_ENUM_TRANSLATION_BASE(, src_t, dst_t, num_elem, src_elems, dst_elems)

#define USAGI_ENUM_TRANSLATION_NS(ns, src_t, dst_t, num_elem, src_elems, dst_elems) \
USAGI_ENUM_TRANSLATION_BASE(ns::, src_t, dst_t, num_elem, src_elems, dst_elems)
