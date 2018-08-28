#pragma once

#include <memory>

namespace usagi
{
class GpuImage;
class GpuDevice;

bool isStbImageSupportedFormat(const char *file_extension);

/**
 * \brief Import JPG, PNG, TGA, BMP, PSD, GIF, HDR, PIC as GpuImage
 */
std::shared_ptr<GpuImage> stbImageImportAsGpuImage(
    GpuDevice *device, std::istream &in);
}
