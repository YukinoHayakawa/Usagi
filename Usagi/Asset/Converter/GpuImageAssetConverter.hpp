#pragma once

#include <memory>

#include <Usagi/Asset/Decoder/StbImageAssetDecoder.hpp>
#include <Usagi/Runtime/Graphics/GpuImage.hpp>

namespace usagi
{
class AssetRoot;
struct ImageBuffer;
class GpuDevice;

struct GpuImageAssetConverter
{
    using DefaultDecoder = StbImageAssetDecoder;

    std::shared_ptr<GpuImage> operator()(
        AssetRoot *assets,
        const ImageBuffer &buffer,
        GpuDevice *device) const;
};
}
