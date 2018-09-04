#pragma once

#include <string>

namespace usagi
{
class Asset;
class AssetRoot;

struct AssetLoadingContext
{
    AssetRoot *asset_root = nullptr;
    Asset *asset = nullptr;
    std::string locator;
};
}
