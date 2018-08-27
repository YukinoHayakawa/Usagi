#pragma once

#include <vulkan/vulkan.hpp>

#include <Usagi/Utility/EnumTranslator.hpp>

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
enum class CompareOp;

namespace vulkan
{
vk::ShaderStageFlagBits translate(ShaderStage stage);
vk::VertexInputRate translate(VertexInputRate rate);
vk::PrimitiveTopology translate(PrimitiveTopology topology);
vk::CullModeFlags translate(FaceCullingMode face_culling_mode);
vk::FrontFace translate(FrontFace front_face);
vk::PolygonMode translate(PolygonMode polygon_mode);
vk::BlendOp translate(BlendingOperation blending_operation);
vk::BlendFactor translate(BlendingFactor blending_factor);
vk::AttachmentLoadOp translate(GpuAttachmentLoadOp op);
vk::AttachmentStoreOp translate(GpuAttachmentStoreOp op);
vk::IndexType translate(GraphicsIndexType type);
vk::SampleCountFlagBits translateSampleCount(std::uint32_t sample_count);
vk::PipelineStageFlagBits translate(GraphicsPipelineStage stage);
vk::BufferUsageFlagBits translate(GpuBufferUsage usage);
vk::SamplerAddressMode translate(GpuSamplerAddressMode mode);
vk::Filter translate(GpuFilter filter);
vk::ComponentSwizzle translate(GpuImageComponentSwizzle swizzle);

USAGI_ENUM_TRANSLATION_DECL(GpuBufferFormat, vk::Format);
USAGI_ENUM_TRANSLATION_DECL(GpuImageUsage, vk::ImageUsageFlagBits);
USAGI_ENUM_TRANSLATION_DECL(CompareOp, vk::CompareOp);
USAGI_ENUM_TRANSLATION_DECL(GpuImageLayout, vk::ImageLayout);
}
}
