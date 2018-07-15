#include "FilesystemAssetPackage.hpp"

usagi::Asset * usagi::FilesystemAssetPackage::findByUuid(
    const boost::uuids::uuid &uuid)
{
    const auto iter = mUuidMap.find(uuid);
    if(iter == mUuidMap.end())
        throw std::runtime_error("Asset not found.");
    const AssetPath path { iter->second };
    return findByPath(path);
}

usagi::Asset * usagi::FilesystemAssetPackage::findByPath(const AssetPath &path)
{
    const auto package_name = path.package();
    if(!package_name.empty() && package_name != name())
        return nullptr;

    return Asset::findByPath(path);
}
