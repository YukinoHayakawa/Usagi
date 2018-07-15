#include "Asset.hpp"

bool usagi::Asset::acceptChild(Element *child)
{
    // Ensure all children are Asset instances.
    return is_instance_of<Asset>(child);
}

usagi::Asset::Asset(Element *parent, std::string name, boost::uuids::uuid uuid)
    : Element { parent, std::move(name) }
    , mUuid { uuid }
{
}
