#include "Asset.hpp"

#include <Usagi/Engine/Core/Logging.hpp>

void usagi::Asset::checkLoaded() const
{
    if(!loaded())
        LOG(critical, "Asset is not loaded!");
}

usagi::Asset::Asset(Element *parent, std::string name, boost::uuids::uuid uuid)
    : ElementTreeNode { parent, std::move(name) }
    , mUuid { uuid }
{
}
