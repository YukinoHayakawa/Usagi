﻿#include "AssetRoot.hpp"

#include <boost/uuid/string_generator.hpp>

#include "AssetPackage.hpp"
#include "Asset.hpp"

usagi::AssetRoot::AssetRoot(Element *parent)
    : Element { parent, "AssetRoot" }
{
}

bool usagi::AssetRoot::acceptChild(Element *child)
{
    return is_instance_of<AssetPackage>(child);
}

usagi::Asset * usagi::AssetRoot::findAssetByUuid(
    const boost::uuids::uuid &uuid) const
{
    for(auto iter = childrenBegin(); iter != childrenEnd(); ++iter)
    {
        auto pkg = static_cast<AssetPackage*>(iter->get());
        if(const auto asset = pkg->findByUuid(uuid))
            return asset;
    }
    return nullptr;
}

usagi::Asset * usagi::AssetRoot::findAssetByString(
    std::string string) const
{
    const auto colon_pos = string.find_first_of(':');
    if(colon_pos != std::string::npos) // package name is specified
    {
        string[colon_pos] = 0; // sperate package name and path
        const std::string package_name { string.c_str() };
        const std::string path { string.c_str() + colon_pos + 1};

        if(const auto pkg = findChildByName(package_name))
        {
            return static_cast<AssetPackage*>(pkg)->findByString(path);
        }
		return nullptr;
    }
    // search each package linearly
	for(auto iter = childrenBegin(); iter != childrenEnd(); ++iter)
	{
		auto pkg = static_cast<AssetPackage*>(iter->get());
		if(const auto asset = pkg->findByString(string))
			return asset;
	}
	return nullptr;
}

usagi::Asset * usagi::AssetRoot::findAsset(std::string locator) const
{
    Asset *asset = nullptr;
    if(locator.empty())
        throw std::runtime_error("Empty asset locator cannot match any asset.");
    if(locator.front() == '{') // try UUID
        asset = findAssetByUuid(boost::uuids::string_generator()(locator));
	else
        asset = findAssetByString(std::move(locator));
    if(asset == nullptr)
        throw std::runtime_error("Asset not found.");
    if(!asset->loaded())
        asset->load();
    return asset;
}