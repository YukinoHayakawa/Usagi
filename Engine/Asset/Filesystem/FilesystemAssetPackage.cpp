﻿#include "FilesystemAssetPackage.hpp"

#include <Usagi/Engine/Graphics/Shader/SpirvBinary.hpp>
#include <Usagi/Engine/Asset/Asset.hpp>

#include "FilesystemAsset.hpp"

usagi::Asset * usagi::FilesystemAssetPackage::findByUuid(
    const boost::uuids::uuid &uuid)
{
    const auto iter = mUuidMap.find(uuid);
    if(iter == mUuidMap.end())
        throw std::runtime_error("Asset not found.");
    return findByFilesystemPath(iter->second);
}

usagi::Asset * usagi::FilesystemAssetPackage::findByString(
    const std::string &string)
{
    return findByFilesystemPath(string);
}

usagi::Asset * usagi::FilesystemAssetPackage::findByFilesystemPath(
    const std::filesystem::path &relative_path)
{
	// extract relative path
    const auto normalized = relative_path.relative_path().lexically_normal();
    {
		if(normalized.empty())
			throw std::runtime_error("Empty path.");
        if(!normalized.has_extension())
            throw std::runtime_error("File extension is missing.");
        const auto &first_component = *normalized.begin();
		if(first_component == ".")
		    throw std::runtime_error("Path does not point to a file.");
		if(first_component == "..")
		    throw std::runtime_error("Path must be within the root folder.");
    }
	// search in cache
	if(const auto asset = findChildByName(normalized.string()))
	{
	    return static_cast<Asset*>(asset);
	}
	// not in cache, create a child as proxy and return it.
    // the asset won't be loaded until load() is invoked on it.
    {
        return addChild<FilesystemAsset>(normalized.string());
    }
}

usagi::FilesystemAssetPackage::FilesystemAssetPackage(
    Element *parent,
    std::string name,
    std::filesystem::path root_path)
    : AssetPackage { parent, std::move(name) }
    , mRootPath { std::move(root_path) }
{
}
