#include "AssetRoot.hpp"

#include <boost/uuid/string_generator.hpp>
#include <boost/uuid/uuid_io.hpp>

#include <Usagi/Core/Logging.hpp>

#include "AssetPackage.hpp"
#include "Asset.hpp"

usagi::AssetRoot::AssetRoot(Element *parent)
    : Element { parent, "AssetRoot" }
{
}

usagi::Asset * usagi::AssetRoot::findAssetByUuid(
    const boost::uuids::uuid &uuid) const
{
    LOG(info, "Searching asset by UUID: {}", boost::uuids::to_string(uuid));
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
        string[colon_pos] = 0; // separate package name and path
        // c_str is essential to the correct segmentation by the \0.
        const std::string package_name { string.c_str() };
        const std::string path { string.c_str() + colon_pos + 1};

        LOG(info, "Searching asset in package {}: {}", package_name, path);
        if(const auto pkg =
            static_cast<AssetPackage*>(findChildByName(package_name)))
        {
            return pkg->findByString(path);
        }
        return nullptr;
    }

    LOG(info, "Searching asset in all packages: {}", string);
    // search each package linearly
    for(auto iter = childrenBegin(); iter != childrenEnd(); ++iter)
    {
        auto pkg = static_cast<AssetPackage*>(iter->get());
        try
        {
            if(const auto asset = pkg->findByString(string))
                return asset;
        }
        catch(const std::exception &e)
        {
            LOG(error,
                "An exception was thrown while searching package %s: %s",
                pkg->name(), e.what()
            );
        }
    }
    return nullptr;
}

bool usagi::AssetRoot::acceptChild(Element *child)
{
    return is_instance_of<AssetPackage>(child);
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
