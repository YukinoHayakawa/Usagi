#pragma once

#include <Usagi/Engine/Asset/Asset.hpp>

namespace usagi
{
class FilesystemAsset : public Asset
{
    AssetType mType = AssetType::UNINITIALIZED;

public:
    FilesystemAsset(Element *parent, std::string name);

    void load() override;

    AssetType assetType() override { return mType; }
};
}
