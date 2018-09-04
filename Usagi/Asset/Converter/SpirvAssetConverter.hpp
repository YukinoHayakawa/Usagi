#pragma once

#include <memory>
#include <filesystem>
#include <optional>

#include <Usagi/Asset/Decoder/RawAssetDecoder.hpp>
#include <Usagi/Runtime/Graphics/Shader/SpirvBinary.hpp>

namespace usagi
{
struct AssetLoadingContext;

struct SpirvAssetConverter
{
    using DefaultDecoder = RawAssetDecoder;

    std::shared_ptr<SpirvBinary> operator()(
        AssetLoadingContext *ctx,
        std::istream &in,
        ShaderStage stage,
        const std::optional<std::filesystem::path> & cache_folder =
            "./.cache/shader"
    ) const;
};
}
