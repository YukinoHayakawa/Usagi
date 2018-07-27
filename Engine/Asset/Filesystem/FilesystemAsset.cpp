#include "FilesystemAsset.hpp"

#include <fstream>

#include <Usagi/Engine/Core/Logging.hpp>
#include <Usagi/Engine/Runtime/Graphics/Shader/SpirvBinary.hpp>

#include "FilesystemAssetPackage.hpp"

usagi::FilesystemAsset::FilesystemAsset(Element *parent, std::string name)
    : Asset { parent, std::move(name) }
{
}

void usagi::FilesystemAsset::load()
{
    const auto full_path =
        static_cast<FilesystemAssetPackage*>(parent())->rootPath() / name();
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
        mPayload = SpirvBinary::fromGlslSourceStream(in, ShaderStage::VERTEX);
        mType = AssetType::VERTEX_SHADER;
    }
    else if(ext == ".frag")
    {
        mPayload = SpirvBinary::fromGlslSourceStream(in, ShaderStage::FRAGMENT);
        mType = AssetType::FRAGMENT_SHADER;
    }
    else { throw std::runtime_error("Unsupported file format."); }
}
