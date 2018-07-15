#pragma once

#include <istream>
#include <memory>

namespace usagi
{
class Asset;

class AssetImporter
{
public:
    virtual ~AssetImporter() = default;

    virtual std::unique_ptr<Asset> import(std::istream &data_source) = 0;
};
}
