#pragma once

#include <Usagi/Core/Math.hpp>
#include <Usagi/Utility/Noncopyable.hpp>

#include "Enum/GpuImageLayout.hpp"
#include "Enum/GraphicsIndexType.hpp"
#include "Enum/GraphicsPipelineStage.hpp"
#include "Shader/ShaderStage.hpp"
#include "ShaderResource.hpp"
#include "GraphicsPipeline.hpp"

namespace usagi
{
class GraphicsPipeline;
class GpuImage;
class GpuBuffer;
class Framebuffer;

/**
 * \brief Device states including resource bindings are reset between
 * command lists.
 */
class GraphicsCommandList : Noncopyable
{
public:
    virtual ~GraphicsCommandList() = default;

    virtual void beginRecording() = 0;
    virtual void endRecording() = 0;

    // Setup commands
    // Cannot be used between beginRender() and endRender().

    // todo: demo purpose only. too complicated.
    virtual void imageTransition(
        GpuImage *image,
        GpuImageLayout old_layout,
        GpuImageLayout new_layout,
        GraphicsPipelineStage src_stage,
        GraphicsPipelineStage dest_stage
    ) = 0;
    virtual void clearColorImage(
        GpuImage *image,
        GpuImageLayout layout,
        Color4f color
    ) = 0;

    // Graphics commands

    virtual void beginRendering(
        std::shared_ptr<RenderPass> render_pass,
        std::shared_ptr<Framebuffer> framebuffer
    ) = 0;
    virtual void endRendering() = 0;

    /**
     * \brief Binding the same pipeline as the current one should be optimized
     * as a no-op.
     * \param pipeline
     */
    virtual void bindPipeline(std::shared_ptr<GraphicsPipeline> pipeline) = 0;

    // Dynamic States

    /**
     * \brief
     * \param index
     * \param origin Upper-left corner
     * \param size
     */
    virtual void setViewport(
        std::uint32_t index,
        Vector2f origin,
        Vector2f size
	) = 0;

    virtual void setScissor(
        std::uint32_t viewport_index,
        Vector2i32 origin,
        Vector2u32 size
	) = 0;

    virtual void setLineWidth(float width) = 0;

    // Resource

    /**
     * \brief Set the value of field located in a special uniform buffer
     * called constant buffer.
     * e.g. Push constants in Vulkan, constants in root signatures in DX12.
     * Before calling this method, there must be an active pipeline bind
     * previously using current command buffer so that the layout of constant
     * can be determined.
     * \param stage The shader stage within which the name is defined.
     * \param name The name of the field which is extracted from the shaders
     * of the active pipeline.
     * \param data The source buffer to be read from, containing the data
     * for writing into the variable.
     * \param size A validation field for preventing over-reading the source
     * buffer. This must equal to the size of variable declared in the shaders.
     */
    virtual void setConstant(
        ShaderStage stage,
        const char *name,
        const void *data,
        std::size_t size
	) = 0;

    virtual void bindIndexBuffer(
        const std::shared_ptr<GpuBuffer> &buffer,
        std::size_t offset,
        GraphicsIndexType type
    ) = 0;

    /**
     * \brief
     * \param binding_index
     * \param buffer Passed in as pointer since it is only a wrapper of the
     * allocated memory, which is the real tracked resource.
     * \param offset
     */
    virtual void bindVertexBuffer(
        std::uint32_t binding_index,
        const std::shared_ptr<GpuBuffer> &buffer,
        std::size_t offset
    ) = 0;

    // Shader Resource Binding

    /**
     * \brief Update shader resource set. Amount and types of resources must
     * match with the shader.
     * \param set_id
     * \param resources
     */
    virtual void bindResourceSet(
        std::uint32_t set_id,
        std::initializer_list<std::shared_ptr<ShaderResource>> resources
    ) = 0;

    // Draw

    virtual void drawInstanced(
        std::uint32_t vertex_count,
        std::uint32_t instance_count,
        std::uint32_t first_vertex,
        std::uint32_t first_instance
    ) = 0;

    /**
     * \brief Note: indices, per-vertex and per-instance data are in buffers.
     * \param index_count Number of indices for reading vertices of each
     * instance
     * \param instance_count Number of instances
     * \param first_index
     * \param vertex_offset A value added to each index
     * \param first_instance
     */
    virtual void drawIndexedInstanced(
        std::uint32_t index_count,
        std::uint32_t instance_count,
        std::uint32_t first_index,
        std::int32_t vertex_offset,
        std::uint32_t first_instance
	) = 0;
};
}
