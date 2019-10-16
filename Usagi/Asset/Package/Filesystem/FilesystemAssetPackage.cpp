#include "FilesystemAssetPackage.hpp"

#include <Usagi/Runtime/Graphics/Shader/SpirvBinary.hpp>
#include <Usagi/Asset/Asset.hpp>
#include <Usagi/Core/Logging.hpp>
#include <Usagi/Utility/Unicode.hpp>

#include "FilesystemAsset.hpp"

usagi::Asset * usagi::FilesystemAssetPackage::findByUuid(
    const boost::uuids::uuid &uuid)
{
    const auto iter = mUuidMap.find(uuid);
    if(iter == mUuidMap.end())
        USAGI_THROW(std::runtime_error("Asset not found."));
    return findByFilesystemPath(iter->second);
}

usagi::Asset * usagi::FilesystemAssetPackage::findByString(
    const std::string &string)
{
    return findByFilesystemPath(stringToU8string(string));
}

usagi::Asset * usagi::FilesystemAssetPackage::findByFilesystemPath(
    const std::filesystem::path &relative_path)
{
    // extract relative path
    const auto normalized = relative_path.relative_path().lexically_normal();
    {
        if(normalized.empty())
            USAGI_THROW(std::runtime_error("Empty path."));
        const auto &first_component = *normalized.begin();
        if(first_component == ".")
            USAGI_THROW(std::runtime_error("Path does not point to a file."));
        if(first_component == "..")
            USAGI_THROW(std::runtime_error("Path must be within the root folder."));
    }
    // search in cache
    if(const auto asset = findChild(u8stringToString(
        normalized.u8string())))
    {
        return static_cast<Asset*>(asset);
    }
    // not in cache, create a child and return it if the file exists.
    {
        const auto asset_path = mRootPath / normalized;
        if(!exists(asset_path))
        {
            if(is_symlink(asset_path))
                USAGI_THROW(std::runtime_error(
                    "Symlink points to invalid location"));
            return nullptr;
        }
        return addChild<FilesystemAsset>(u8stringToString(
            normalized.u8string()));
    }
}

bool usagi::FilesystemAssetPackage::acceptChild(Element *child)
{
    return is_instance_of<FilesystemAsset>(child);
}

usagi::FilesystemAssetPackage::FilesystemAssetPackage(
    Element *parent,
    std::string name,
    std::filesystem::path root_path)
    : AssetPackage { parent, std::move(name) }
    , mRootPath { std::move(root_path) }
{
    LOG(info, "FilesystemAssetPackage: {}",
        std::filesystem::canonical(mRootPath));
}
