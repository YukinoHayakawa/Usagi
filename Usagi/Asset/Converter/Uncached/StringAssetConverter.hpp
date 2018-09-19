#pragma once

#include <string>

#include <Usagi/Asset/Decoder/RawAssetDecoder.hpp>

namespace usagi
{
struct AssetLoadingContext;

struct StringAssetConverter
{
    using DefaultDecoder = RawAssetDecoder;

    std::string operator()(
        AssetLoadingContext *ctx,
        std::istream &in
    ) const;
};
}
