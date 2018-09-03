#include "SpirvAssetConverter.hpp"

#include <Usagi/Runtime/Graphics/Shader/SpirvBinary.hpp>

std::shared_ptr<usagi::SpirvBinary> usagi::SpirvAssetConverter::operator()(
    AssetRoot *assets,
    std::istream &in,
    const ShaderStage stage,
    const std::optional<std::filesystem::path> & cache_folder) const
{
    return SpirvBinary::fromGlslSourceStream(in, stage, cache_folder);
}
