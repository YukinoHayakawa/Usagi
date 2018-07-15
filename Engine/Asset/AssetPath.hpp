#pragma once

#include <string>
#include <vector>
#include <filesystem>

namespace usagi
{
class AssetPath
{
    std::string mPackage;
    using ComponentArray = std::vector<std::string>;
    ComponentArray mComponents;

public:
    AssetPath(const std::string &path);
    AssetPath(const char *path);
    AssetPath(const std::filesystem::path &path);

    std::string package() const { return mPackage; }

    std::size_t numComponents() const { return mComponents.size(); }

    ComponentArray::const_iterator componentBegin() const
    {
        return mComponents.begin();
    }

    ComponentArray::const_iterator componentEnd() const
    {
        return mComponents.end();
    }

    /**
     * \brief Return a path of the same package but with components before
     * first_component removed.
     * \param first_component 
     * \return 
     */
    AssetPath subPath(std::size_t first_component) const
    {
        
    }

    explicit operator std::filesystem::path() const;
};
}
