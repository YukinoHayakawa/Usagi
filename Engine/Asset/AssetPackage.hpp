#pragma once

#include "Asset.hpp"

namespace usagi
{
/**
 * \brief Stored shared resources. All assets are wrapped in a shared_ptr.
 */
class AssetPackage : public Asset
{
public:
    virtual ~AssetPackage() = default;

    AssetType assetType() override;
    Asset * findByUuid(const boost::uuids::uuid &uuid) override;
    Asset * findByPath(const AssetPath &path) override;

private:
	bool acceptChild(Element *child) override;
};
}
