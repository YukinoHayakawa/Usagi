#include "VulkanSampler.hpp"

usagi::VulkanSampler::VulkanSampler(vk::UniqueSampler vk_sampler)
    : mSampler(std::move(vk_sampler))
{
}

void usagi::VulkanSampler::fillShaderResourceInfo(
    vk::WriteDescriptorSet &write,
    VulkanResourceInfo &info)
{
    auto &image_info = std::get<vk::DescriptorImageInfo>(info);
    image_info.setSampler(mSampler.get());
    write.setPImageInfo(&image_info);
}
