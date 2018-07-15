#include "AssetPackage.hpp"

#include <Usagi/Engine/Utility/TypeCast.hpp>

#include "Asset.hpp"

bool usagi::AssetPackage::acceptChild(Element *child)
{
    return is_instance_of<Asset>(child);
}

usagi::AssetPackage::AssetPackage(Element *parent, std::string name)
    : Element{ parent, std::move(name) }
{
}
