#pragma once

#include <Eigen/Core>

#include <Usagi/Engine/Utility/Noncopyable.hpp>
#include <Usagi/Engine/Core/Math.hpp>

namespace usagi
{
class GraphicsPipeline;

class GraphicsCommandList : Noncopyable
{
public:
    virtual ~GraphicsCommandList() = default;

    virtual void startRecording() = 0;
    virtual void finishRecording() = 0;

	virtual void bindPipeline(
        GraphicsPipeline *pipeline
	) = 0;

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
        Vector2f size) = 0;

    virtual void setScissor(
        std::uint32_t viewport_index,
        Vector2i32 origin,
        Vector2u32 size) = 0;

    // Resource

    /**
     * \brief Set the value of speficied variable embedded in the command buffer.
     * e.g. Push constants in Vulkan, constants in root signatures in DX12.
     * Before calling this method, there must be an active pipeline bind
     * previously using current command buffer so that the layout of constant
     * can be determined.
     * \param name The name of the field which is extracted from the shaders
     * of the active pipeline.
     * \param data The source buffer to be read from, containing the data
     * for writing into the variable.
     * \param size A validation field for preventing overreading the source
     * buffer. This must equal to the size of variable declared in the shaders.
     */
    virtual void setConstant(
        const char *name,
        const void *data,
        std::size_t size
    ) = 0;

    // Draw

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
        std::uint32_t first_instance) = 0;
};
}
