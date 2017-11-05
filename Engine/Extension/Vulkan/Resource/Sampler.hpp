#pragma once

#include <vulkan/vulkan.hpp>

#include <Usagi/Engine/Runtime/Graphics/Resource/Sampler.hpp>
#include <Usagi/Engine/Runtime/Graphics/Resource/SamplerCreateInfo.hpp>

namespace yuki::vulkan
{

class Sampler : public graphics::Sampler
{
    vk::UniqueSampler mSampler;

public:
    static vk::Filter translateFilter(graphics::SamplerCreateInfo::Filter filter);
    static vk::SamplerAddressMode translateAddressMode(graphics::SamplerCreateInfo::AddressMode mode);
    
    static std::unique_ptr<Sampler> create(class Device *vulkan_gd, const graphics::SamplerCreateInfo &info);
};

}
