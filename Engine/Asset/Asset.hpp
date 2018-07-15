#pragma once

#include <boost/uuid/uuid.hpp>

#include <Usagi/Engine/Core/Element.hpp>
#include <Usagi/Engine/Utility/TypeCast.hpp>

#include "AssetType.hpp"
#include "AssetPath.hpp"

namespace usagi
{
class Asset : public Element
{
    boost::uuids::uuid mUuid;
    std::any mPayload;

    bool acceptChild(Element *child) override;

public:
    Asset(Element *parent, std::string name, boost::uuids::uuid uuid);

    boost::uuids::uuid uuid() const { return mUuid; }
    virtual AssetType assetType() { return AssetType::DIRECTORY; }

    virtual bool loaded() const { return mPayload.has_value(); }
    virtual void load() { }
    virtual void unload() { }

    /**
     * \brief Package root should maintain a mapping between uuids and assets.
     * \param uuid 
     * \return 
     */
    virtual Asset * findByUuid(const boost::uuids::uuid &uuid)
    {
        return uuid == mUuid ? this : nullptr;
    }

    virtual Asset * findByPath(const AssetPath &path);

    /**
     * \brief Cast payload to stored type. AssetT must match the resource
     * type exactly, same as typically the base type of the resource.
     * \tparam AssetT 
     * \return 
     */
    template <typename AssetT>
    std::shared_ptr<AssetT> as()
    {
        return std::any_cast<std::shared_ptr<AssetT>>(mPayload);
    }

	template <typename AssetT, typename DerivedAssetT>
	std::shared_ptr<DerivedAssetT> as(const std::string &name)
	{
		return dynamic_pointer_cast_throw<DerivedAssetT>(as<AssetT>(name));
	}
};
}
