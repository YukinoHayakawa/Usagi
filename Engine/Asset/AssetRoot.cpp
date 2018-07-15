#include "AssetRoot.hpp"
#include "AssetPackage.hpp"

bool usagi::AssetRoot::acceptChild(Element *child)
{
    return dynamic_cast<AssetPackage*>(child) != nullptr;
}

usagi::AssetRoot::AssetRoot(Element *parent)
	: Asset { parent, "AssetRoot", { } }
{
}
