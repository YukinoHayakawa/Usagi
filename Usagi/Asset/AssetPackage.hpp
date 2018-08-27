#pragma once

#include <boost/uuid/uuid.hpp>

#include <Usagi/Core/Element.hpp>

namespace usagi
{
class Asset;

class AssetPackage : public Element
{
    bool acceptChild(Element *child) override;

public:
    AssetPackage(Element *parent, std::string name);
    virtual ~AssetPackage() = default;

    virtual Asset * findByUuid(const boost::uuids::uuid &uuid) = 0;
    virtual Asset * findByString(const std::string &string) = 0;
};
}
