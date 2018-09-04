#include "JsonPropertySheetAssetConverter.hpp"

#include <stack>

#include <Usagi/Asset/AssetRoot.hpp>
#include <Usagi/Core/Logging.hpp>

namespace usagi::moeloop
{
json JsonCascadingLoader::mergedJson(AssetLoadingContext *ctx, json j)
{
    std::stack<json> inheritance; // bottom->top = derived->parent
    inheritance.push(std::move(j));

    // follow inheritance pointers
    auto current_asset = ctx->asset;
    while(true)
    {
        const auto inherit = inheritance.top().find("inherit");
        if(inherit == inheritance.top().end()) break;

        const auto parent_locator = current_asset->parentPath() +
            inherit.value().get<std::string>();
        // get parent asset
        current_asset = ctx->asset_root->findAsset(parent_locator);
        if(!current_asset)
        {
            LOG(error, "Failed to load referenced json: {}", parent_locator);
            throw std::runtime_error("Asses reference error");
        }
        // push parent json
        inheritance.push(current_asset->decode<DefaultDecoder>());
    }

    // merge from base to derived
    json ret;
    while(!inheritance.empty())
    {
        ret.merge_patch(inheritance.top());
        inheritance.pop();
    }
    ret.erase("inherit");
    return std::move(ret);
}
}
