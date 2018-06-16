#pragma once

#include <string>
#include <memory>
#include <any>

#include <Usagi/Engine/Utility/Noncopyable.hpp>

namespace yuki
{
class Asset;

/**
 * \brief Stored shared resources. All assets are wrapped in a shared_ptr.
 */
class AssetPackage : Noncopyable
{
public:
    virtual ~AssetPackage() = default;

    // assets should be stored using their common based types, no matter
    // how were them created, so that they can be accessed without knowing
    // the exact type. if the user think he knows the real type, he can
    // use the next template to perform a dynamic cast.
    template <typename AssetT>
    std::shared_ptr<AssetT> getAsset(const std::string &name)
    {
        return std::any_cast<std::shared_ptr<AssetT>>(getAssetAny(name));
    }

    template <typename AssetT, typename DerivedAssetT>
    std::shared_ptr<DerivedAssetT> getAsset(const std::string &name)
    {
        auto p = std::dynamic_pointer_cast<DerivedAssetT>(
            getAsset<AssetT>(name)
        );
        if(p == nullptr)
            throw std::bad_cast();
        return std::move(p);
    }

private:
    virtual std::any getAssetAny(const std::string &name) = 0;
};
}
