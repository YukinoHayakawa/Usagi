#pragma once

#include <memory>

#include "JSON.hpp"
#include "JsonAssetDecoder.hpp"

namespace usagi
{
struct AssetLoadingContext;
}

namespace usagi::moeloop
{
struct JsonCascadingLoader
{
    using DefaultDecoder = JsonAssetDecoder;

    static json mergedJson(AssetLoadingContext *ctx, json j);
};

struct JsonPropertySheetAssetConverter : JsonCascadingLoader
{
    template <typename... Args>
    json operator()(AssetLoadingContext *ctx, json j) const
    {
        return mergedJson(ctx, std::move(j));
    }
};
}
