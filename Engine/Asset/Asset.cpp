#include "Asset.hpp"

#include <loguru.hpp>

bool usagi::Asset::acceptChild(Element *child)
{
    // Ensure all children are Asset instances.
    return is_instance_of<Asset>(child);
}

void usagi::Asset::checkLoaded() const
{
    if(!loaded())
        ABORT_F("Asset is not loaded!");
}

usagi::Asset::Asset(Element *parent, std::string name, boost::uuids::uuid uuid)
    : Element { parent, std::move(name) }
    , mUuid { uuid }
{
}
