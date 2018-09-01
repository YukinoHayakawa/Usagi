#pragma once

#include <filesystem>

#include <Usagi/Asset/AssetPackage.hpp>

namespace usagi
{
class Runtime;

class FilesystemAssetPackage : public AssetPackage
{
    std::map<boost::uuids::uuid, std::filesystem::path> mUuidMap;
    std::filesystem::path mRootPath;

    Asset * findByFilesystemPath(const std::filesystem::path &relative_path);

    bool acceptChild(Element *child) override;

public:
    FilesystemAssetPackage(
        Element *parent,
        std::string name,
        std::filesystem::path root_path
    );

    Asset * findByUuid(const boost::uuids::uuid &uuid) override;
    Asset * findByString(const std::string &string) override;

    std::filesystem::path rootPath() const { return mRootPath; }
};
}
