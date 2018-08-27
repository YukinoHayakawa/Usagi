#include "AssetPackage.hpp"

#include <Usagi/Utility/TypeCast.hpp>

bool usagi::AssetPackage::acceptChild(Element *child)
{
    return is_instance_of<AssetPackage>(child);
}

usagi::AssetPackage::AssetPackage(Element *parent, std::string name)
    : Element { parent, std::move(name) }
{
}
