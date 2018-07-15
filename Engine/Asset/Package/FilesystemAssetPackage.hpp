#pragma once

#include <filesystem>

#include <Usagi/Engine/Asset/AssetPackage.hpp>

namespace usagi
{
class FilesystemAssetPackage : public AssetPackage
{
    std::map<boost::uuids::uuid, std::filesystem::path> mUuidMap;
    
    std::any getAssetAny(const std::string &name) override;

public:
    AssetType assetType() override { return AssetType::PACKAGE; }
    Asset * findByUuid(const boost::uuids::uuid &uuid) override;
    Asset * findByPath(const AssetPath &path) override;
};
}
