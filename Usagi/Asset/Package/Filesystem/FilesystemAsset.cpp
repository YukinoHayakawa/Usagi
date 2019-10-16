#include "FilesystemAsset.hpp"

#include <fstream>

#include <Usagi/Core/Logging.hpp>
#include <Usagi/Utility/Unicode.hpp>
#include <Usagi/Runtime/Runtime.hpp>

#include "FilesystemAssetPackage.hpp"

usagi::FilesystemAsset::FilesystemAsset(
    Element *parent,
    std::string name)
    : Asset { parent, std::move(name) }
{
    assert(is_instance_of<FilesystemAssetPackage>(parent));
}

usagi::FilesystemAssetPackage * usagi::FilesystemAsset::package() const
{
    return static_cast<FilesystemAssetPackage*>(parent());
}

std::string usagi::FilesystemAsset::path() const
{
    return fmt::format("{}:{}", package()->name(), name());
}

std::string usagi::FilesystemAsset::parentPath() const
{
    const std::filesystem::path path { stringToU8string(name()) };
    return fmt::format("{}:{}/",
        package()->name(), u8stringToString(path.parent_path().u8string()));
}

std::unique_ptr<std::istream> usagi::FilesystemAsset::open()
{
    const auto full_path =
        package()->rootPath() / stringToU8string(name());

    LOG(info, "Loading asset: {}", name());

    auto in = std::make_unique<std::ifstream>(
        full_path,
        std::ios_base::binary | std::ios_base::in);
    in->exceptions(std::ios::badbit);

    if(!*in)
    {
        LOG(error, "Failed to open {}", u8stringToString(full_path.u8string()));
        USAGI_THROW(std::runtime_error("Failed to open file."));
    }

    return std::move(in);
}
