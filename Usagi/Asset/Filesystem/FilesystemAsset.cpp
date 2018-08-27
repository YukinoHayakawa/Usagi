#include "FilesystemAsset.hpp"

#include <fstream>

#include <Usagi/Core/Logging.hpp>
#include <Usagi/Runtime/Graphics/Shader/SpirvBinary.hpp>

#include "FilesystemAssetPackage.hpp"

usagi::FilesystemAsset::FilesystemAsset(
    Element *parent,
    std::string name)
    : Asset { parent, std::move(name) }
{
    assert(is_instance_of<FilesystemAssetPackage>(parent));
}

void usagi::FilesystemAsset::load()
{
    // todo file handlers by extension name
    const auto pkg = static_cast<FilesystemAssetPackage*>(parent());
    const auto full_path = pkg->rootPath() / name();
    const auto cache_path = pkg->rootPath() / ".cache";
    const auto ext = full_path.extension();
    std::ifstream in { full_path, std::ios::binary };
    in.exceptions(std::ios::badbit | std::ios::failbit);

    if(!in)
    {
        LOG(error, "Failed to open {}", full_path);
        throw std::runtime_error("Failed to open file.");
    }

    if(ext == ".vert")
    {
        mPayload = SpirvBinary::fromGlslSourceStream(
            in, ShaderStage::VERTEX, cache_path);
        mType = AssetType::VERTEX_SHADER;
    }
    else if(ext == ".frag")
    {
        mPayload = SpirvBinary::fromGlslSourceStream(
            in, ShaderStage::FRAGMENT, cache_path);
        mType = AssetType::FRAGMENT_SHADER;
    }
    else
    {
        throw std::runtime_error("Unsupported file format.");
    }
}
