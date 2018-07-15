#include "FilesystemAsset.hpp"

#include <fstream>

#include <Usagi/Engine/Graphics/Shader/SpirvBinary.hpp>

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
    in.exceptions(std::ios::badbit);

    if(ext == ".vert")
        mPayload = SpirvBinary::fromGlslSourceStream(in, ShaderStage::VERTEX);
	else if(ext == ".frag")
		mPayload = SpirvBinary::fromGlslSourceStream(in, ShaderStage::FRAGMENT);
	else
        throw std::runtime_error("Unsupported file format.");
}
