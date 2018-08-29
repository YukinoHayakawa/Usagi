#pragma once

#include <memory>
#include <string>

namespace usagi
{
class GpuImage;
class GpuDevice;

bool isStbImageSupportedFormat(const std::string &file_extension);

/**
 * \brief Import JPG, PNG, TGA, BMP, PSD, GIF, HDR, PIC as GpuImage
 */
std::shared_ptr<GpuImage> stbImageImportAsGpuImage(
    GpuDevice *device, std::istream &in);
}
