#pragma once

#include <Usagi/Asset/Asset.hpp>
#include "FilesystemAssetPackage.hpp"

namespace usagi
{
class FilesystemAsset : public Asset
{
    FilesystemAssetPackage * package() const;

public:
    FilesystemAsset(Element *parent, std::string name);

    std::unique_ptr<std::istream> open() override;
    std::string path() const override;
    std::string parentPath() const override;
};
}
