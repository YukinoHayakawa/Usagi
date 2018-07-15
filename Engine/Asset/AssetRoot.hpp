#pragma once

#include "Asset.hpp"

namespace usagi
{
class AssetRoot : public Asset
{
	bool acceptChild(Element *child) override;

public:
    explicit AssetRoot(Element *parent);

	Asset * findByUuid(const boost::uuids::uuid &uuid) override;
	Asset * findByPath(const AssetPath &path) override;
};
}
