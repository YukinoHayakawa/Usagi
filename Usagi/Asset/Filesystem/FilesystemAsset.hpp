#pragma once

#include <Usagi/Asset/Asset.hpp>

namespace usagi
{
class FilesystemAssetPackage;

class FilesystemAsset : public Asset
{
    AssetType mType = AssetType::UNINITIALIZED;

public:
    FilesystemAsset(Element *parent, std::string name);

    void load() override;

    AssetType assetType() override { return mType; }
};
}
