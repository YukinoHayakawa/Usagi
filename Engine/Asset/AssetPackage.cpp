#include "AssetPackage.hpp"

#include "Asset.hpp"

usagi::AssetPackage::AssetPackage(Element *parent, std::string name)
    : ElementTreeNode { parent, std::move(name) }
{
}
