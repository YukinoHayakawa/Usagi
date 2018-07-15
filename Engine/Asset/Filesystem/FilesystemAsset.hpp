#pragma once

#include <Usagi/Engine/Asset/Asset.hpp>

namespace usagi
{
class FilesystemAsset : public Asset
{
public:
    FilesystemAsset(Element *parent, std::string name);

    void load() override;
};
}
