#include "VulkanEnumTranslation.hpp"

#include <Usagi/Runtime/Graphics/Enum/CompareOp.hpp>
#include <Usagi/Runtime/Graphics/Enum/GpuBufferUsage.hpp>
#include <Usagi/Runtime/Graphics/Enum/GpuImageUsage.hpp>
#include <Usagi/Runtime/Graphics/Enum/GraphicsIndexType.hpp>
#include <Usagi/Runtime/Graphics/Enum/GraphicsPipelineStage.hpp>
#include <Usagi/Runtime/Graphics/PipelineCreateInfo.hpp>
#include <Usagi/Runtime/Graphics/RenderPassCreateInfo.hpp>
#include <Usagi/Runtime/Graphics/GpuImageViewCreateInfo.hpp>
#include <Usagi/Runtime/Graphics/GpuSamplerCreateInfo.hpp>
#include <Usagi/Runtime/Graphics/Shader/ShaderStage.hpp>

using namespace usagi;

// todo all use USAGI_ENUM_TRANSLATION
namespace usagi::vulkan
{
vk::ShaderStageFlagBits translate(ShaderStage stage)
{
    switch(stage)
    {
        case ShaderStage::VERTEX:
            return vk::ShaderStageFlagBits::eVertex;
        case ShaderStage::FRAGMENT:
            return vk::ShaderStageFlagBits::eFragment;
        default:
            throw std::runtime_error("Invalid shader stage");
    }
}

GpuBufferFormat from(vk::Format format)
{
    switch(format)
    {
        case vk::Format::eR8G8B8A8Unorm:
            return GpuBufferFormat::R8G8B8A8_UNORM;
        case vk::Format::eR32Sfloat:
            return GpuBufferFormat::R32_SFLOAT;
        case vk::Format::eR32G32Sfloat:
            return GpuBufferFormat::R32G32_SFLOAT;
        case vk::Format::eR32G32B32Sfloat:
            return GpuBufferFormat::R32G32B32_SFLOAT;
        case vk::Format::eR32G32B32A32Sfloat:
            return GpuBufferFormat::R32G32B32A32_SFLOAT;
        default:
            throw std::runtime_error("Unimplemented vk::Format.");
    }
}

vk::VertexInputRate translate(VertexInputRate rate)
{
    switch(rate)
    {
        case VertexInputRate::PER_VERTEX:
            return vk::VertexInputRate::eVertex;
        case VertexInputRate::PER_INSTANCE:
            return vk::VertexInputRate::eInstance;
        default:
            throw std::runtime_error("Invalid vertex input rate.");
    }
}

vk::PrimitiveTopology translate(PrimitiveTopology topology)
{
    switch(topology)
    {
        case PrimitiveTopology::POINT_LIST:
            return vk::PrimitiveTopology::ePointList;
        case PrimitiveTopology::LINE_LIST:
            return vk::PrimitiveTopology::eLineList;
        case PrimitiveTopology::LINE_STRIP:
            return vk::PrimitiveTopology::eLineStrip;
        case PrimitiveTopology::TRIANGLE_LIST:
            return vk::PrimitiveTopology::eTriangleList;
        case PrimitiveTopology::TRIANGLE_STRIP:
            return vk::PrimitiveTopology::eTriangleStrip;
        case PrimitiveTopology::TRIANGLE_FAN:
            return vk::PrimitiveTopology::eTriangleFan;
        default:
            throw std::runtime_error("Invalid primitive topology.");
    }
}

vk::CullModeFlags translate(FaceCullingMode face_culling_mode)
{
    switch(face_culling_mode)
    {
        case FaceCullingMode::NONE:
            return vk::CullModeFlagBits::eNone;
        case FaceCullingMode::FRONT:
            return vk::CullModeFlagBits::eFront;
        case FaceCullingMode::BACK:
            return vk::CullModeFlagBits::eBack;
        default:
            throw std::runtime_error("Invalid face culling mode.");
    }
}

vk::FrontFace translate(FrontFace front_face)
{
    switch(front_face)
    {
        case FrontFace::CLOCKWISE:
            return vk::FrontFace::eClockwise;
        case FrontFace::COUNTER_CLOCKWISE:
            return vk::FrontFace::eCounterClockwise;
        default:
            throw std::runtime_error("Invalid front face.");
    }
}

vk::PolygonMode translate(PolygonMode polygon_mode)
{
    switch(polygon_mode)
    {
        case PolygonMode::FILL:
            return vk::PolygonMode::eFill;
        case PolygonMode::LINE:
            return vk::PolygonMode::eLine;
        case PolygonMode::POINT:
            return vk::PolygonMode::ePoint;
        default:
            throw std::runtime_error("Invalid polygon mode.");
    }
}

vk::BlendOp translate(BlendingOperation blending_operation)
{
    switch(blending_operation)
    {
        case BlendingOperation::ADD:
            return vk::BlendOp::eAdd;
        case BlendingOperation::SUBTRACT:
            return vk::BlendOp::eSubtract;
        case BlendingOperation::REVERSE_SUBTRACT:
            return vk::BlendOp::eReverseSubtract;
        case BlendingOperation::MIN:
            return vk::BlendOp::eMin;
        case BlendingOperation::MAX:
            return vk::BlendOp::eMax;
        default:
            throw std::runtime_error("Invalid blending operation.");
    }
}

vk::BlendFactor translate(BlendingFactor blending_factor)
{
    switch(blending_factor)
    {
        case BlendingFactor::ZERO:
            return vk::BlendFactor::eZero;
        case BlendingFactor::ONE:
            return vk::BlendFactor::eOne;
        case BlendingFactor::SOURCE_ALPHA:
            return vk::BlendFactor::eSrcAlpha;
        case BlendingFactor::ONE_MINUS_SOURCE_ALPHA:
            return vk::BlendFactor::eOneMinusSrcAlpha;
        default:
            throw std::runtime_error("Invalid blending factor.");
    }
}

vk::AttachmentLoadOp translate(GpuAttachmentLoadOp op)
{
    switch(op)
    {
        case GpuAttachmentLoadOp::LOAD:
            return vk::AttachmentLoadOp::eLoad;
        case GpuAttachmentLoadOp::CLEAR:
            return vk::AttachmentLoadOp::eClear;
        case GpuAttachmentLoadOp::UNDEFINED:
            return vk::AttachmentLoadOp::eDontCare;
        default:
            throw std::runtime_error("Invalid GpuAttachmentLoadOp.");
    }
}

vk::AttachmentStoreOp translate(GpuAttachmentStoreOp op)
{
    switch(op)
    {
        case GpuAttachmentStoreOp::STORE:
            return vk::AttachmentStoreOp::eStore;
        case GpuAttachmentStoreOp::UNDEFINED:
            return vk::AttachmentStoreOp::eDontCare;
        default:
            throw std::runtime_error("Invalid GpuAttachmentStoreOp.");
    }
}

vk::IndexType translate(GraphicsIndexType type)
{
    switch(type)
    {
        case GraphicsIndexType::UINT16:
            return vk::IndexType::eUint16;
        case GraphicsIndexType::UINT32:
            return vk::IndexType::eUint32;
        default:
            throw std::runtime_error("Invalid index type.");
    }
}

vk::SampleCountFlagBits translateSampleCount(std::uint32_t sample_count)
{
    switch(sample_count)
    {
        case 1: return vk::SampleCountFlagBits::e1;
        case 2: return vk::SampleCountFlagBits::e2;
        case 4: return vk::SampleCountFlagBits::e4;
        case 8: return vk::SampleCountFlagBits::e8;
        case 16: return vk::SampleCountFlagBits::e16;
        case 32: return vk::SampleCountFlagBits::e32;
        case 64: return vk::SampleCountFlagBits::e64;
        default: throw std::runtime_error("Invalid sample count.");
    }
}

vk::PipelineStageFlagBits translate(GraphicsPipelineStage stage)
{
    switch(stage)
    {
        case GraphicsPipelineStage::TOP_OF_PIPE:
            return vk::PipelineStageFlagBits::eTopOfPipe;
        case GraphicsPipelineStage::VERTEX_INPUT:
            return vk::PipelineStageFlagBits::eVertexInput;
        case GraphicsPipelineStage::VERTEX_SHADER:
            return vk::PipelineStageFlagBits::eVertexShader;
        case GraphicsPipelineStage::GEOMETRY_SHADER:
            return vk::PipelineStageFlagBits::eGeometryShader;
        case GraphicsPipelineStage::FRAGMENT_SHADER:
            return vk::PipelineStageFlagBits::eFragmentShader;
        case GraphicsPipelineStage::EARLY_FRAGMENT_TESTS:
            return vk::PipelineStageFlagBits::eEarlyFragmentTests;
        case GraphicsPipelineStage::LATE_FRAGMENT_TESTS:
            return vk::PipelineStageFlagBits::eLateFragmentTests;
        case GraphicsPipelineStage::COLOR_ATTACHMENT_OUTPUT:
            return vk::PipelineStageFlagBits::eColorAttachmentOutput;
        case GraphicsPipelineStage::TRANSFER:
            return vk::PipelineStageFlagBits::eTransfer;
        case GraphicsPipelineStage::BOTTOM_OF_PIPE:
            return vk::PipelineStageFlagBits::eBottomOfPipe;
        case GraphicsPipelineStage::HOST:
            return vk::PipelineStageFlagBits::eHost;
        default: throw std::runtime_error("Invalid GraphicsPipelineStage.");
    }
}

vk::BufferUsageFlagBits translate(const GpuBufferUsage usage)
{
    switch(usage)
    {
        case GpuBufferUsage::VERTEX:
            return vk::BufferUsageFlagBits::eVertexBuffer;
        case GpuBufferUsage::INDEX:
            return vk::BufferUsageFlagBits::eIndexBuffer;
        case GpuBufferUsage::UNIFORM:
            return vk::BufferUsageFlagBits::eUniformBuffer;
        default: throw std::runtime_error("Invalid GpuBufferUsage.");
    }
}

vk::SamplerAddressMode translate(GpuSamplerAddressMode mode)
{
    switch(mode)
    {
        case GpuSamplerAddressMode::REPEAT:
            return vk::SamplerAddressMode::eRepeat;
        case GpuSamplerAddressMode::MIRRORED_REPEAT:
            return vk::SamplerAddressMode::eMirroredRepeat;
        case GpuSamplerAddressMode::CLAMP_TO_EDGE:
            return vk::SamplerAddressMode::eClampToEdge;
        case GpuSamplerAddressMode::CLAMP_TO_BORDER:
            return vk::SamplerAddressMode::eClampToBorder;
        case GpuSamplerAddressMode::MIRROR_CLAMP_TO_EDGE:
            return vk::SamplerAddressMode::eMirrorClampToEdge;
        default: throw std::runtime_error("Invalid GpuSamplerAddressMode.");
    }
}

vk::Filter translate(const GpuFilter filter)
{
    switch(filter)
    {
        case GpuFilter::NEAREST: return vk::Filter::eNearest;
        case GpuFilter::LINEAR: return vk::Filter::eLinear;
        default: throw std::runtime_error("Invalid GpuFilter.");
    }
}

vk::ComponentSwizzle translate(const GpuImageComponentSwizzle swizzle)
{
    switch(swizzle)
    {
        case GpuImageComponentSwizzle::IDENTITY:
            return vk::ComponentSwizzle::eIdentity;
        case GpuImageComponentSwizzle::ZERO:
            return vk::ComponentSwizzle::eZero;
        case GpuImageComponentSwizzle::ONE:
            return vk::ComponentSwizzle::eOne;
        case GpuImageComponentSwizzle::R:
            return vk::ComponentSwizzle::eR;
        case GpuImageComponentSwizzle::G:
            return vk::ComponentSwizzle::eG;
        case GpuImageComponentSwizzle::B:
            return vk::ComponentSwizzle::eB;
        case GpuImageComponentSwizzle::A:
            return vk::ComponentSwizzle::eA;
        default: throw std::runtime_error("Invalid GpuImageComponentSwizzle.");
    }
}
}

USAGI_ENUM_TRANSLATION_NS(usagi::vulkan, GpuBufferFormat, vk::Format, 14,
    (
        GpuBufferFormat::R8_UNORM,
        GpuBufferFormat::R8G8_UNORM,
        GpuBufferFormat::R8G8B8_UNORM,
        GpuBufferFormat::R8G8B8A8_UNORM,
        GpuBufferFormat::R32_SFLOAT,
        GpuBufferFormat::R32G32_SFLOAT,
        GpuBufferFormat::R32G32B32_SFLOAT,
        GpuBufferFormat::R32G32B32A32_SFLOAT,
        GpuBufferFormat::D16_UNORM,
        GpuBufferFormat::D32_SFLOAT,
        GpuBufferFormat::D16_UNORM_S8_UINT,
        GpuBufferFormat::D24_UNORM_S8_UINT,
        GpuBufferFormat::D32_SFLOAT_S8_UINT,
    ),
    (
        vk::Format::eR8Unorm,
        vk::Format::eR8G8Unorm,
        vk::Format::eR8G8B8Unorm,
        vk::Format::eR8G8B8A8Unorm,
        vk::Format::eR32Sfloat,
        vk::Format::eR32G32Sfloat,
        vk::Format::eR32G32B32Sfloat,
        vk::Format::eR32G32B32A32Sfloat,
        vk::Format::eD16Unorm,
        vk::Format::eD32Sfloat,
        vk::Format::eD16UnormS8Uint,
        vk::Format::eD24UnormS8Uint,
        vk::Format::eD32SfloatS8Uint,
    )
)

USAGI_ENUM_TRANSLATION_NS(usagi::vulkan, GpuImageUsage, vk::ImageUsageFlagBits, 4,
(
    GpuImageUsage::SAMPLED,
    GpuImageUsage::COLOR_ATTACHMENT,
    GpuImageUsage::DEPTH_STENCIL_ATTACHMENT,
    GpuImageUsage::INPUT_ATTACHMENT,
), (
    vk::ImageUsageFlagBits::eSampled,
    vk::ImageUsageFlagBits::eColorAttachment,
    vk::ImageUsageFlagBits::eDepthStencilAttachment,
    vk::ImageUsageFlagBits::eInputAttachment,
))

USAGI_ENUM_TRANSLATION_NS(usagi::vulkan, CompareOp, vk::CompareOp, 8,
(
    CompareOp::NEVER,
    CompareOp::LESS,
    CompareOp::EQUAL,
    CompareOp::LESS_OR_EQUAL,
    CompareOp::GREATER,
    CompareOp::NOT_EQUAL,
    CompareOp::GREATER_OR_EQUAL,
    CompareOp::ALWAYS,
), (
    vk::CompareOp::eNever,
    vk::CompareOp::eLess,
    vk::CompareOp::eEqual,
    vk::CompareOp::eLessOrEqual,
    vk::CompareOp::eGreater,
    vk::CompareOp::eNotEqual,
    vk::CompareOp::eGreaterOrEqual,
    vk::CompareOp::eAlways,
))

USAGI_ENUM_TRANSLATION_NS(usagi::vulkan, GpuImageLayout, vk::ImageLayout, 8,
(
    GpuImageLayout::UNDEFINED,
    GpuImageLayout::PRESENT,
    GpuImageLayout::TRANSFER_SRC,
    GpuImageLayout::TRANSFER_DST,
    GpuImageLayout::COLOR_ATTACHMENT,
    GpuImageLayout::DEPTH_STENCIL_ATTACHMENT,
    GpuImageLayout::SHADER_READ_ONLY,
    GpuImageLayout::PREINITIALIZED,
), (
    vk::ImageLayout::eUndefined,
    vk::ImageLayout::ePresentSrcKHR,
    vk::ImageLayout::eTransferSrcOptimal,
    vk::ImageLayout::eTransferDstOptimal,
    vk::ImageLayout::eColorAttachmentOptimal,
    vk::ImageLayout::eDepthStencilAttachmentOptimal,
    vk::ImageLayout::eShaderReadOnlyOptimal,
    vk::ImageLayout::ePreinitialized,
))
