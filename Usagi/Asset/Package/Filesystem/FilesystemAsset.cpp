#include "FilesystemAsset.hpp"

#include <fstream>

#include <Usagi/Core/Logging.hpp>
#include <Usagi/Runtime/Runtime.hpp>

#include "FilesystemAssetPackage.hpp"

usagi::FilesystemAsset::FilesystemAsset(
    Element *parent,
    std::string name)
    : Asset { parent, std::move(name) }
{
    assert(is_instance_of<FilesystemAssetPackage>(parent));
}

std::unique_ptr<std::istream> usagi::FilesystemAsset::open()
{
    const auto pkg = static_cast<FilesystemAssetPackage*>(parent());
    const auto full_path = pkg->rootPath() / std::filesystem::u8path(name());

    LOG(info, "Loading asset: {}", name());

    auto in = std::make_unique<std::ifstream>(
        full_path,
        std::ios_base::binary | std::ios_base::in);
    in->exceptions(std::ios::badbit);

    if(!*in)
    {
        LOG(error, "Failed to open {}", full_path.u8string());
        throw std::runtime_error("Failed to open file.");
    }

    return std::move(in);
}
