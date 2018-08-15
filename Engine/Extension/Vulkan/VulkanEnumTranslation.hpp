#pragma once

#include <vulkan/vulkan.hpp>

namespace usagi
{
enum class GpuImageComponentSwizzle;
enum class GpuFilter;
enum class GpuSamplerAddressMode;
enum class GpuImageUsage;
enum class GpuBufferUsage;
enum class GraphicsPipelineStage;
enum class GpuBufferFormat;
enum class GraphicsIndexType;
enum class GpuImageLayout;
enum class GpuAttachmentStoreOp;
enum class GpuAttachmentLoadOp;
enum class VertexInputRate;
enum class PrimitiveTopology;
enum class FaceCullingMode;
enum class FrontFace;
enum class PolygonMode;
enum class BlendingOperation;
enum class BlendingFactor;
enum class ShaderStage;

namespace vulkan
{
vk::ShaderStageFlagBits translate(ShaderStage stage);
vk::Format translate(GpuBufferFormat format);
GpuBufferFormat from(vk::Format format);
vk::VertexInputRate translate(VertexInputRate rate);
vk::PrimitiveTopology translate(PrimitiveTopology topology);
vk::CullModeFlags translate(FaceCullingMode face_culling_mode);
vk::FrontFace translate(FrontFace front_face);
vk::PolygonMode translate(PolygonMode polygon_mode);
vk::BlendOp translate(BlendingOperation blending_operation);
vk::BlendFactor translate(BlendingFactor blending_factor);
vk::AttachmentLoadOp translate(GpuAttachmentLoadOp op);
vk::AttachmentStoreOp translate(GpuAttachmentStoreOp op);
vk::ImageLayout translate(GpuImageLayout layout);
vk::IndexType translate(GraphicsIndexType type);
vk::SampleCountFlagBits translateSampleCount(std::uint32_t sample_count);
vk::PipelineStageFlagBits translate(GraphicsPipelineStage stage);
vk::BufferUsageFlagBits translate(GpuBufferUsage usage);
vk::ImageUsageFlagBits translate(GpuImageUsage usage);
vk::SamplerAddressMode translate(GpuSamplerAddressMode mode);
vk::Filter translate(GpuFilter filter);
vk::ComponentSwizzle translate(GpuImageComponentSwizzle swizzle);
}
}
