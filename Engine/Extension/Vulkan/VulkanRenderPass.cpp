#include "VulkanRenderPass.hpp"

#include <Usagi/Engine/Runtime/Graphics/RenderPassCreateInfo.hpp>

#include "VulkanEnumTranslation.hpp"
#include "VulkanGpuDevice.hpp"

usagi::VulkanRenderPass::VulkanRenderPass(
    VulkanGpuDevice *device,
    const RenderPassCreateInfo &info)
{
    vk::RenderPassCreateInfo vk_info;

    std::vector<vk::AttachmentDescription> attachment_descriptions;
    attachment_descriptions.reserve(info.attachment_usages.size());
    for(auto &&u : info.attachment_usages)
    {
        vk::AttachmentDescription d;
        d.setFormat(translate(u.format));
        d.setSamples(translateSampleCount(u.sample_count));
        d.setInitialLayout(translate(u.initial_layout));
        d.setFinalLayout(translate(u.final_layout));
        d.setLoadOp(translate(u.load_op));
        d.setStoreOp(translate(u.store_op));
        attachment_descriptions.push_back(d);
    }
    vk_info.setAttachmentCount(
        static_cast<uint32_t>(attachment_descriptions.size()));
    vk_info.setPAttachments(attachment_descriptions.data());

    vk::SubpassDescription subpass;
    std::vector<vk::AttachmentReference> attachment_references;
    attachment_references.reserve(info.attachment_usages.size());
    for(std::size_t i = 0; i < info.attachment_usages.size(); ++i)
    {
        vk::AttachmentReference r;
        r.setAttachment(static_cast<uint32_t>(i));
        r.setLayout(translate(info.attachment_usages[i].layout));
        attachment_references.push_back(r);
    }
    subpass.setPipelineBindPoint(vk::PipelineBindPoint::eGraphics);
    subpass.setColorAttachmentCount(
        static_cast<uint32_t>(attachment_references.size()));
    subpass.setPColorAttachments(attachment_references.data());

    vk_info.setSubpassCount(1);
    vk_info.setPSubpasses(&subpass);

    mRenderPass = device->device().createRenderPassUnique(vk_info);
}
