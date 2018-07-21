#include "Asset.hpp"

#include <Usagi/Engine/Core/Logging.hpp>

bool usagi::Asset::acceptChild(Element *child)
{
    // Ensure all children are Asset instances.
    return is_instance_of<Asset>(child);
}

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
