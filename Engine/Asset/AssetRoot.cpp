#include "AssetRoot.hpp"

#include <loguru.hpp>
#include <boost/uuid/string_generator.hpp>
#include <boost/uuid/uuid_io.hpp>

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
    LOG_F(INFO, "Searching asset by UUID: %s",
        boost::uuids::to_string(uuid).c_str()
    );
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

        LOG_F(INFO, "Searching asset in package %s: %s",
            package_name.c_str(),
            path.c_str()
        );
        if(const auto pkg = findChildByName(package_name))
        {
            return static_cast<AssetPackage*>(pkg)->findByString(path);
        }
        return nullptr;
    }

    LOG_F(INFO, "Searching asset in all packages: %s",string.c_str());
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
            LOG_F(INFO,
                "An exception was thrown while searching package %s: %s",
                pkg->name().c_str(),
                e.what()
            );
        }
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
