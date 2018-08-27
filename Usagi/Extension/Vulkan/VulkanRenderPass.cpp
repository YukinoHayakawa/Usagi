#include "VulkanRenderPass.hpp"

#include <Usagi/Core/Logging.hpp>
#include <Usagi/Runtime/Graphics/RenderPassCreateInfo.hpp>

#include "VulkanEnumTranslation.hpp"
#include "VulkanGpuDevice.hpp"

using namespace usagi::vulkan;

usagi::VulkanRenderPass::VulkanRenderPass(
    VulkanGpuDevice *device,
    const RenderPassCreateInfo &info)
{
    mClearValues.reserve(info.attachment_usages.size());

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
        mClearValues.emplace_back(std::array<float, 4> {
            u.clear_color.x(), u.clear_color.y(),
            u.clear_color.z(), u.clear_color.w()
        });
    }
    vk_info.setAttachmentCount(
        static_cast<uint32_t>(attachment_descriptions.size()));
    vk_info.setPAttachments(attachment_descriptions.data());

    vk::SubpassDescription subpass;
    std::vector<vk::AttachmentReference> color_refs;
    vk::AttachmentReference ds_ref;
    for(std::size_t i = 0; i < info.attachment_usages.size(); ++i)
    {
        auto &&a = info.attachment_usages[i];
        if(a.layout == GpuImageLayout::DEPTH_STENCIL_ATTACHMENT)
        {
            if(subpass.pDepthStencilAttachment)
                LOG(error, "Only one depth stencil attachmend may be used.");
            ds_ref.setAttachment(static_cast<uint32_t>(i));
            ds_ref.setLayout(translate(a.layout));
            subpass.setPDepthStencilAttachment(&ds_ref);
        }
        else
        {
            vk::AttachmentReference r;
            r.setAttachment(static_cast<uint32_t>(i));
            r.setLayout(translate(a.layout));
            color_refs.push_back(r);
        }
    }
    subpass.setPipelineBindPoint(vk::PipelineBindPoint::eGraphics);
    subpass.setColorAttachmentCount(
        static_cast<uint32_t>(color_refs.size()));
    subpass.setPColorAttachments(color_refs.data());

    vk_info.setSubpassCount(1);
    vk_info.setPSubpasses(&subpass);

    mRenderPass = device->device().createRenderPassUnique(vk_info);
}
