#include <Usagi/Engine/Runtime/Exception.hpp>
#include <Usagi/Engine/Extension/Vulkan/Device/Device.hpp>

#include "Sampler.hpp"

vk::Filter yuki::vulkan::Sampler::translateFilter(graphics::SamplerCreateInfo::Filter filter)
{
    switch(filter)
    {
        case graphics::SamplerCreateInfo::Filter::NEAREST: return vk::Filter::eNearest;
        case graphics::SamplerCreateInfo::Filter::LINEAR: return vk::Filter::eLinear;
        default: throw GraphicsAPIEnumTranslationException() << EnumInfo("vk::Filter");
    }
}

vk::SamplerAddressMode yuki::vulkan::Sampler::translateAddressMode(graphics::SamplerCreateInfo::AddressMode mode)
{
    switch(mode)
    {
        case graphics::SamplerCreateInfo::AddressMode::REPEAT: return vk::SamplerAddressMode::eRepeat;
        case graphics::SamplerCreateInfo::AddressMode::MIRRORED_REPEAT: return vk::SamplerAddressMode::eMirroredRepeat;
        case graphics::SamplerCreateInfo::AddressMode::CLAMP_TO_EDGE: return vk::SamplerAddressMode::eClampToEdge;
        case graphics::SamplerCreateInfo::AddressMode::CLAMP_TO_BORDER: return vk::SamplerAddressMode::eClampToBorder;
        default: throw GraphicsAPIEnumTranslationException() << EnumInfo("vk::SamplerAddressMode");
    }
}

std::unique_ptr<yuki::vulkan::Sampler> yuki::vulkan::Sampler::create(Device *vulkan_gd,
    const graphics::SamplerCreateInfo &info)
{
    vk::SamplerCreateInfo sampler_create_info;
    sampler_create_info.setMagFilter(translateFilter(info.magnify_filter));
    sampler_create_info.setMinFilter(translateFilter(info.minify_filter));
    sampler_create_info.setAddressModeU(translateAddressMode(info.address_mode_u));
    sampler_create_info.setAddressModeV(translateAddressMode(info.address_mode_v));
    sampler_create_info.setAddressModeW(vk::SamplerAddressMode::eClampToEdge);
    sampler_create_info.setBorderColor(vk::BorderColor::eFloatTransparentBlack);
    sampler_create_info.setMipmapMode(vk::SamplerMipmapMode::eLinear);
    sampler_create_info.setMaxAnisotropy(1.f);

    auto sampler = std::make_unique<Sampler>();
    sampler->mSampler = vulkan_gd->_getDevice().createSamplerUnique(sampler_create_info);
    return std::move(sampler);
}
