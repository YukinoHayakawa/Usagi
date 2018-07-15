#include "Asset.hpp"

bool usagi::Asset::acceptChild(Element *child)
{
    // Ensure all children are Asset instances.
    return dynamic_cast<Asset*>(child) != nullptr;
}

usagi::Asset::Asset(Element *parent, std::string name, boost::uuids::uuid uuid)
    : Element { parent, std::move(name) }
    , mUuid { std::move(uuid) }
{
}

usagi::Asset * usagi::Asset::findByPath(const AssetPath &path)
{
    switch(path.numComponents())
    {
        case 0:
            // Empty path does not match anything.
            return nullptr;

        case 1:
            // Exact match
            return *path.componentBegin() == name() ? this : nullptr;

        default:
        {
            // Unmatched root folder name.
            if(*path.componentBegin() != name())
                return nullptr;
            auto subpath = path.subPath(1);
            // No components left - dead end.
            if(subpath.numComponents() == 0)
                return nullptr;

            // Search recursively into sub-directories
            for(auto iter = childrenBegin(); iter != childrenEnd(); ++iter)
            {
                auto &e = static_cast<Asset&>(*iter->get());
                const auto r = e.findByPath(subpath);
                if(r)
                    return r;
            }
            return nullptr;
        }
    }
}
