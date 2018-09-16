#include "VulkanGraphicsCommandList.hpp"

#include <Usagi/Utility/TypeCast.hpp>

#include "VulkanGpuDevice.hpp"
#include "VulkanEnumTranslation.hpp"
#include "VulkanRenderPass.hpp"
#include "VulkanHelper.hpp"
#include "VulkanGpuBuffer.hpp"
#include "VulkanGpuImage.hpp"
#include "VulkanFramebuffer.hpp"
#include "VulkanGpuCommandPool.hpp"
#include "VulkanBufferAllocation.hpp"
#include "VulkanMemoryPool.hpp"
#include "VulkanGraphicsPipeline.hpp"
#include "VulkanGpuImageView.hpp"
#include "VulkanShaderResource.hpp"

using namespace usagi::vulkan;

usagi::VulkanGraphicsCommandList::VulkanGraphicsCommandList(
    std::shared_ptr<VulkanGpuCommandPool> pool,
    vk::UniqueCommandBuffer vk_command_buffer)
    : mCommandPool(std::move(pool))
    , mCommandBuffer(std::move(vk_command_buffer))
{
}

void usagi::VulkanGraphicsCommandList::beginRecording()
{
    vk::CommandBufferBeginInfo command_buffer_begin_info;
    command_buffer_begin_info.setFlags(
        vk::CommandBufferUsageFlagBits::eOneTimeSubmit);

    mCommandBuffer->begin(command_buffer_begin_info);
}

void usagi::VulkanGraphicsCommandList::endRecording()
{
    mCommandBuffer->end();
}

void usagi::VulkanGraphicsCommandList::imageTransition(
    GpuImage *image,
    GpuImageLayout old_layout,
    GpuImageLayout new_layout,
    GraphicsPipelineStage src_stage,
    GraphicsPipelineStage dest_stage)
{
    auto &vk_image = dynamic_cast_ref<VulkanGpuImage>(image);

    vk::ImageMemoryBarrier barrier;
    barrier.setImage(vk_image.image());
    barrier.setOldLayout(translate(old_layout));
    barrier.setNewLayout(translate(new_layout));
    const auto queue_family_index =
        mCommandPool->device()->graphicsQueueFamily();
    barrier.setSrcQueueFamilyIndex(queue_family_index);
    barrier.setDstQueueFamilyIndex(queue_family_index);

    // modified from
    // https://harrylovescode.gitbooks.io/vulkan-api/content/chap07/chap07.html
    // todo: not reliable
    using vk::AccessFlagBits;
    switch(barrier.oldLayout)
    {
        case vk::ImageLayout::ePreinitialized:
            barrier.srcAccessMask =
                AccessFlagBits::eHostWrite;
            break;
        case vk::ImageLayout::eColorAttachmentOptimal:
            barrier.srcAccessMask =
                AccessFlagBits::eColorAttachmentWrite;
            break;
        case vk::ImageLayout::eDepthStencilAttachmentOptimal:
            barrier.srcAccessMask =
                AccessFlagBits::eDepthStencilAttachmentWrite;
            break;
        case vk::ImageLayout::eTransferSrcOptimal:
            barrier.srcAccessMask = AccessFlagBits::eTransferRead;
            break;
        case vk::ImageLayout::eShaderReadOnlyOptimal:
            barrier.srcAccessMask = AccessFlagBits::eShaderRead;
            break;
        //case vk::ImageLayout::ePresentSrcKHR:
        //    barrier.srcAccessMask = AccessFlagBits::eMemoryRead;
        //    break;
        default: ;
    }
    switch(barrier.newLayout)
    {
        case vk::ImageLayout::eTransferDstOptimal:
            barrier.dstAccessMask = AccessFlagBits::eTransferWrite;
            break;
        case vk::ImageLayout::eTransferSrcOptimal:
            barrier.srcAccessMask |= AccessFlagBits::eTransferRead;
            barrier.dstAccessMask = AccessFlagBits::eTransferRead;
            break;
        case vk::ImageLayout::eColorAttachmentOptimal:
            barrier.dstAccessMask = AccessFlagBits::eColorAttachmentWrite;
            barrier.srcAccessMask = AccessFlagBits::eTransferRead;
            break;
        case vk::ImageLayout::eDepthAttachmentStencilReadOnlyOptimal:
            barrier.dstAccessMask |=
                AccessFlagBits::eDepthStencilAttachmentWrite;
            break;
        case vk::ImageLayout::eShaderReadOnlyOptimal:
            barrier.dstAccessMask = AccessFlagBits::eShaderRead;
            break;
        //case vk::ImageLayout::ePresentSrcKHR:
        //    barrier.dstAccessMask = AccessFlagBits::eMemoryRead;
        //    break;
        default:;
    }
    barrier.subresourceRange.setAspectMask(vk::ImageAspectFlagBits::eColor);
    barrier.subresourceRange.setBaseArrayLayer(0);
    barrier.subresourceRange.setLayerCount(1);
    barrier.subresourceRange.setBaseMipLevel(0);
    barrier.subresourceRange.setLevelCount(1);

    mCommandBuffer->pipelineBarrier(
        translate(src_stage), translate(dest_stage),
        { }, { }, { }, { barrier }
    );
}

// note: bad performance on tile-based GPUs
// https://developer.samsung.com/game/usage#clearingattachments
void usagi::VulkanGraphicsCommandList::clearColorImage(
    GpuImage *image,
    const GpuImageLayout layout,
    Color4f color)
{
    auto &vk_image = dynamic_cast_ref<VulkanGpuImage>(image);
    // todo: depending on image format, float/uint/int should be used
    const vk::ClearColorValue color_value { std::array<float, 4> {
        color.x(), color.y(), color.z(), color.w()
    }};
    vk::ImageSubresourceRange subresource_range;
    subresource_range.setAspectMask(vk::ImageAspectFlagBits::eColor);
    subresource_range.setBaseArrayLayer(0);
    subresource_range.setLayerCount(1);
    subresource_range.setBaseMipLevel(0);
    subresource_range.setLevelCount(1);
    mCommandBuffer->clearColorImage(
        vk_image.image(), translate(layout),
        color_value, { subresource_range }
    );
}

void usagi::VulkanGraphicsCommandList::beginRendering(
    std::shared_ptr<RenderPass> render_pass,
    std::shared_ptr<Framebuffer> framebuffer)
{
    auto vk_framebuffer =
        dynamic_pointer_cast_throw<VulkanFramebuffer>(framebuffer);
    const auto vk_renderpass =
        dynamic_pointer_cast_throw<VulkanRenderPass>(render_pass);

    // todo reuse framebuffers?
    // todo unmatched view amount?
    vk_framebuffer->create(vk_renderpass);

    vk::RenderPassBeginInfo begin_info;
    // todo support clear values
    const auto s = vk_framebuffer->size();
    begin_info.renderArea.extent.width = s.x();
    begin_info.renderArea.extent.height = s.y();
    begin_info.setFramebuffer(vk_framebuffer->framebuffer());
    begin_info.setRenderPass(vk_renderpass->renderPass());
    const auto &clear_values = vk_renderpass->clearValues();
    begin_info.setClearValueCount(static_cast<uint32_t>(clear_values.size()));
    begin_info.setPClearValues(clear_values.data());
    // assuming that only one render pass is used
    mCommandBuffer->beginRenderPass(begin_info, vk::SubpassContents::eInline);

    for(auto &&view : vk_framebuffer->views())
    {
        // todo: merge into ...->appendResources?
        mResources.push_back(view);
        view->appendAdditionalResources(mResources);
    }
    mResources.push_back(std::move(vk_framebuffer));
}

void usagi::VulkanGraphicsCommandList::endRendering()
{
    mCurrentPipeline.reset();
    mCommandBuffer->endRenderPass();
}

void usagi::VulkanGraphicsCommandList::bindPipeline(
    std::shared_ptr<GraphicsPipeline> pipeline)
{
    auto vk_pipeline =
        dynamic_pointer_cast_throw<VulkanGraphicsPipeline>(pipeline);

    if(mCurrentPipeline == vk_pipeline) return;

    mCommandBuffer->bindPipeline(vk::PipelineBindPoint::eGraphics,
        vk_pipeline->pipeline());
    mCurrentPipeline = vk_pipeline;
    mResources.push_back(std::move(vk_pipeline));
}

void usagi::VulkanGraphicsCommandList::createDescriptorPool()
{
    // todo: precise control over allocation size
    vk::DescriptorPoolCreateInfo info;
    info.setMaxSets(16);
    std::initializer_list<vk::DescriptorPoolSize> sizes {
        { vk::DescriptorType::eSampler, 16 },
    { vk::DescriptorType::eSampledImage, 16 },
    { vk::DescriptorType::eUniformBuffer, 16 },
    };
    info.setPoolSizeCount(static_cast<uint32_t>(sizes.size()));
    info.setPPoolSizes(sizes.begin());
    mDescriptorPools.push_back(
        mCommandPool->device()->device().createDescriptorPoolUnique(info));
}

vk::DescriptorSet usagi::VulkanGraphicsCommandList::
    allocateDescriptorSet(const std::uint32_t set_id)
{
    assert(mCurrentPipeline);

    auto allow_new_pool = true;

    if(mDescriptorPools.empty())
    {
        createDescriptorPool();
        allow_new_pool = false;
    }

    while(true) try
    {
        vk::DescriptorSetAllocateInfo info;
        info.setDescriptorPool(mDescriptorPools.back().get());
        info.setDescriptorSetCount(1);
        const auto layout = { mCurrentPipeline->descriptorSetLayout(set_id) };
        info.setPSetLayouts(layout.begin());

        mDescriptorSets.push_back(mCommandPool->device()->device()
            .allocateDescriptorSets(info).front());
        return mDescriptorSets.back();
    }
    catch(const vk::SystemError &e)
    {
        switch(static_cast<vk::Result>(e.code().value()))
        {
            case vk::Result::eErrorFragmentedPool:
            case vk::Result::eErrorOutOfPoolMemory:
                if(!allow_new_pool) throw;
                createDescriptorPool();
                allow_new_pool = false;
            default: break;
        }
    }
}

// todo performance
void usagi::VulkanGraphicsCommandList::bindResourceSet(
    const std::uint32_t set_id,
    std::initializer_list<std::shared_ptr<ShaderResource>> resources)
{
    auto vk_resources = vulkan::transformObjects(resources, [](auto &&r) {
        return dynamic_pointer_cast_throw<VulkanShaderResource>(r);
    });

    const auto desc_set = allocateDescriptorSet(set_id);
    std::vector<vk::WriteDescriptorSet> writes {
        vk_resources.size(), vk::WriteDescriptorSet {}
    };
    std::vector<VulkanResourceInfo> res_infos {
        vk_resources.size(), VulkanResourceInfo { }
    };
    for(std::size_t i = 0; i < writes.size(); ++i)
    {
        auto &res = vk_resources[i];
        auto batch_res = dynamic_pointer_cast_throw<VulkanBatchResource>(res);
        auto &write = writes[i];
        write.setDescriptorType(mCurrentPipeline->descriptorType(
            set_id, static_cast<uint32_t>(i)));
        write.setDstSet(desc_set);
        write.setDstBinding(static_cast<uint32_t>(i));
        write.setDstArrayElement(0);
        write.setDescriptorCount(1);
        res->fillShaderResourceInfo(write, res_infos[i]);
        batch_res->appendAdditionalResources(mResources);
        mResources.push_back(std::move(batch_res));
    }

    mCommandPool->device()->device().updateDescriptorSets(writes, { });
    mCommandBuffer->bindDescriptorSets(
        vk::PipelineBindPoint::eGraphics,
        mCurrentPipeline->layout(),
        set_id, { desc_set }, { }
    );
}

void usagi::VulkanGraphicsCommandList::setViewport(
    const std::uint32_t index,
    Vector2f origin, Vector2f size)
{
    vk::Viewport viewports[] = {
        { origin.x(), origin.y(), size.x(), size.y(), 0.f, 1.f }
    };
    mCommandBuffer->setViewport(index, 1, viewports);
}

void usagi::VulkanGraphicsCommandList::setScissor(
    const std::uint32_t viewport_index,
    Vector2i32 origin,
    Vector2u32 size)
{
    vk::Rect2D scissor {
        { origin.x(), origin.y() },
        { size.x(), size.y() }
    };
    mCommandBuffer->setScissor(viewport_index, 1, &scissor);
}

void usagi::VulkanGraphicsCommandList::setLineWidth(float width)
{
    mCommandBuffer->setLineWidth(width);
}

void usagi::VulkanGraphicsCommandList::setConstant(
    const ShaderStage stage,
    const char *name,
    const void *data,
    const std::size_t size)
{
    if(mCurrentPipeline == nullptr)
        throw std::runtime_error(
            "No active pipeline is bound, unable to retrieve pipeline layout.");

    const auto constant_info = mCurrentPipeline->queryConstantInfo(stage, name);
    if(size != constant_info.size)
	    throw std::runtime_error("Unmatched constant size.");

    mCommandBuffer->pushConstants(
        mCurrentPipeline->layout(),
        translate(stage),
        constant_info.offset, constant_info.size,
        data
    );
}

void usagi::VulkanGraphicsCommandList::bindIndexBuffer(
    const std::shared_ptr<GpuBuffer> &buffer,
	const std::size_t offset,
	const GraphicsIndexType type)
{
	auto &vk_buffer = dynamic_cast_ref<VulkanGpuBuffer>(buffer.get());
    auto allocation = vk_buffer.allocation();

    mCommandBuffer->bindIndexBuffer(
        allocation->pool()->buffer(), allocation->offset() + offset,
        translate(type)
	);

    mResources.push_back(std::move(allocation));
}

void usagi::VulkanGraphicsCommandList::bindVertexBuffer(
	const std::uint32_t binding_index,
    const std::shared_ptr<GpuBuffer> &buffer,
	const std::size_t offset)
{
	auto &vk_buffer = dynamic_cast_ref<VulkanGpuBuffer>(buffer.get());
    auto allocation = vk_buffer.allocation();

	vk::Buffer buffers[] = { allocation->pool()->buffer() };
    vk::DeviceSize sizes[] = { allocation->offset() + offset };

    mCommandBuffer->bindVertexBuffers(binding_index, 1, buffers, sizes);

    mResources.push_back(std::move(allocation));
}

void usagi::VulkanGraphicsCommandList::drawInstanced(
    const std::uint32_t vertex_count,
    const std::uint32_t instance_count,
    const std::uint32_t first_vertex,
    const std::uint32_t first_instance)
{
    mCommandBuffer->draw(vertex_count, instance_count, first_vertex,
        first_instance);
}

void usagi::VulkanGraphicsCommandList::drawIndexedInstanced(
    const std::uint32_t index_count,
    const std::uint32_t instance_count,
    const std::uint32_t first_index,
    const std::int32_t vertex_offset,
    const std::uint32_t first_instance)
{
    mCommandBuffer->drawIndexed(index_count, instance_count, first_index,
        vertex_offset, first_instance);
}
