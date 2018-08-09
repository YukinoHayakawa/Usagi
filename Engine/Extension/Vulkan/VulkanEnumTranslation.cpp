#include "VulkanEnumTranslation.hpp"

#include <Usagi/Engine/Runtime/Graphics/PipelineCreateInfo.hpp>
#include <Usagi/Engine/Runtime/Graphics/Shader/ShaderStage.hpp>
#include <Usagi/Engine/Runtime/Graphics/Enum/GraphicsIndexType.hpp>
#include <Usagi/Engine/Runtime/Graphics/Enum/GraphicsPipelineStage.hpp>
#include <Usagi/Engine/Runtime/Graphics/Enum/GpuAccess.hpp>
#include <Usagi/Engine/Runtime/Graphics/RenderPassCreateInfo.hpp>

vk::ShaderStageFlagBits usagi::translate(ShaderStage stage)
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

vk::Format usagi::translate(GpuBufferFormat format)
{
    switch(format)
    {
        case GpuBufferFormat::R8G8B8A8_UNORM:
            return vk::Format::eR8G8B8A8Unorm;
        case GpuBufferFormat::R32_SFLOAT:
            return vk::Format::eR32Sfloat;
        case GpuBufferFormat::R32G32_SFLOAT:
            return vk::Format::eR32G32Sfloat;
        case GpuBufferFormat::R32G32B32_SFLOAT:
            return vk::Format::eR32G32B32Sfloat;
        case GpuBufferFormat::R32G32B32A32_SFLOAT:
            return vk::Format::eR32G32B32A32Sfloat;
        default:
            throw std::runtime_error("Unsupported GpuBufferFormat.");
    }
}

usagi::GpuBufferFormat usagi::fromVulkan(vk::Format format)
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

vk::VertexInputRate usagi::translate(VertexInputRate rate)
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

vk::PrimitiveTopology usagi::translate(PrimitiveTopology topology)
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

vk::CullModeFlags usagi::translate(FaceCullingMode face_culling_mode)
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

vk::FrontFace usagi::translate(FrontFace front_face)
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

vk::PolygonMode usagi::translate(PolygonMode polygon_mode)
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

vk::BlendOp usagi::translate(BlendingOperation blending_operation)
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

vk::BlendFactor usagi::translate(BlendingFactor blending_factor)
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

vk::AttachmentLoadOp usagi::translate(GpuAttachmentLoadOp op)
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

vk::AttachmentStoreOp usagi::translate(GpuAttachmentStoreOp op)
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

vk::ImageLayout usagi::translate(GpuImageLayout layout)
{
    switch(layout)
    {
        case GpuImageLayout::UNDEFINED:
            return vk::ImageLayout::eUndefined;
        case GpuImageLayout::PRESENT:
            return vk::ImageLayout::ePresentSrcKHR;
        case GpuImageLayout::TRANSFER_SRC:
            return vk::ImageLayout::eTransferSrcOptimal;
        case GpuImageLayout::TRANSFER_DST:
            return vk::ImageLayout::eTransferDstOptimal;
        case GpuImageLayout::COLOR:
            return vk::ImageLayout::eColorAttachmentOptimal;
        default:
            throw std::runtime_error("Invalid GpuImageLayout.");
    }
}

vk::IndexType usagi::translate(GraphicsIndexType type)
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

vk::SampleCountFlagBits usagi::translateSampleCount(std::uint32_t sample_count)
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

vk::PipelineStageFlagBits usagi::translate(GraphicsPipelineStage stage)
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
        default: throw std::runtime_error("Invalid GraphicsPipelineStage.");
    }
}

vk::AccessFlagBits usagi::translate(GpuAccess access)
{
    switch(access)
    {
        case GpuAccess::INDEX_READ:
            return vk::AccessFlagBits::eIndexRead;
        case GpuAccess::VERTEX_ATTRIBUTE_READ:
            return vk::AccessFlagBits::eVertexAttributeRead;
        case GpuAccess::UNIFORM_READ:
            return vk::AccessFlagBits::eUniformRead;
        case GpuAccess::INPUT_ATTACHMENT_READ:
            return vk::AccessFlagBits::eInputAttachmentRead;
        case GpuAccess::SHADER_READ:
            return vk::AccessFlagBits::eShaderRead;
        case GpuAccess::SHADER_WRITE:
            return vk::AccessFlagBits::eShaderWrite;
        case GpuAccess::COLOR_ATTACHMENT_READ:
            return vk::AccessFlagBits::eColorAttachmentRead;
        case GpuAccess::COLOR_ATTACHMENT_WRITE:
            return vk::AccessFlagBits::eColorAttachmentWrite;
        case GpuAccess::DEPTH_STENCIL_ATTACHMENT_READ:
            return vk::AccessFlagBits::eDepthStencilAttachmentRead;
        case GpuAccess::DEPTH_STENCIL_ATTACHMENT_WRITE:
            return vk::AccessFlagBits::eDepthStencilAttachmentWrite;
        case GpuAccess::TRANSFER_READ:
            return vk::AccessFlagBits::eTransferRead;
        case GpuAccess::TRANSFER_WRITE:
            return vk::AccessFlagBits::eTransferWrite;
        case GpuAccess::HOST_READ:
            return vk::AccessFlagBits::eHostRead;
        case GpuAccess::HOST_WRITE:
            return vk::AccessFlagBits::eHostWrite;
        case GpuAccess::MEMORY_READ:
            return vk::AccessFlagBits::eMemoryRead;
        case GpuAccess::MEMORY_WRITE:
            return vk::AccessFlagBits::eMemoryWrite;
        default:
            throw std::runtime_error("Invalid GpuAccess.");
    }
}
