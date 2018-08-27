#include "Asset.hpp"

#include <Usagi/Core/Logging.hpp>

void usagi::Asset::checkLoaded() const
{
    if(!loaded())
        LOG(critical, "Asset is not loaded!");
}

usagi::Asset::Asset(Element *parent, std::string name, boost::uuids::uuid uuid)
    : Element { parent, std::move(name) }
    , mUuid { uuid }
{
}

usagi::AssetType usagi::Asset::assetType()
{
    return AssetType::DIRECTORY;
}
