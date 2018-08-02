#pragma once

#include <boost/uuid/uuid.hpp>
#include <boost/uuid/nil_generator.hpp>

#include <Usagi/Engine/Core/Element.hpp>
#include <Usagi/Engine/Utility/TypeCast.hpp>

#include "AssetType.hpp"

namespace usagi
{
class Asset : public ElementTreeNode<Element, Asset>
{
    void checkLoaded() const;

protected:
    const boost::uuids::uuid mUuid;
    std::any mPayload;

public:
    Asset(
        Element *parent,
        std::string name,
        boost::uuids::uuid uuid = boost::uuids::nil_uuid()
	);

    boost::uuids::uuid uuid() const { return mUuid; }
    virtual AssetType assetType() { return AssetType::DIRECTORY; }

    virtual bool loaded() const { return mPayload.has_value(); }
    virtual void load() { }
    virtual void unload() { }

    /**
     * \brief Cast payload to stored type. AssetT must match the resource
     * type exactly, same as typically the base type of the resource.
     * \tparam AssetT 
     * \return 
     */
    template <typename AssetT>
    std::shared_ptr<AssetT> as()
    {
        checkLoaded();
        return std::any_cast<std::shared_ptr<AssetT>>(mPayload);
    }

	template <typename AssetT, typename DerivedAssetT>
	std::shared_ptr<DerivedAssetT> as(const std::string &name)
	{
        checkLoaded();
        return dynamic_pointer_cast_throw<DerivedAssetT>(as<AssetT>(name));
	}
};
}
