#pragma once

#include <Usagi/Asset/Asset.hpp>

namespace usagi
{
class FilesystemAsset : public Asset
{
public:
    FilesystemAsset(Element *parent, std::string name);

    std::unique_ptr<std::istream> open() override;
};
}
