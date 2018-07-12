#pragma once

#include <memory>
#include <vector>

#include "GraphicsImageLayout.hpp"
#include "GpuDataFormat.hpp"

namespace usagi
{
struct InputAssemblyState
{
    enum class PrimitiveTopology
    {
        POINT_LIST,
        LINE_LIST,
        LINE_STRIP,
        TRIANGLE_LIST,
        TRIANGLE_STRIP,
        TRIANGLE_FAN,
    } topology = PrimitiveTopology::TRIANGLE_LIST;
};

struct RasterizationState
{
    enum class PolygonMode
    {
        FILL,
        LINE,
        POINT
    } polygon_mode = PolygonMode::FILL;

    enum class FaceCullingMode
    {
        NONE,
        FRONT,
        BACK
    } face_culling_mode = FaceCullingMode::BACK;

    enum class FrontFace
    {
        CLOCKWISE,
        COUNTER_CLOCKWISE
    } front_face = FrontFace::COUNTER_CLOCKWISE;

    // line width?
};

struct DepthStencilState
{
    bool enable_depth_test = true;

    // stencil test
};

struct ColorBlendState
{
    bool enable = false;

    enum class BlendingFactor
    {
        ZERO,
        ONE,
        SOURCE_ALPHA,
        ONE_MINUS_SOURCE_ALPHA,
    } src_color_factor = BlendingFactor::ONE,
      dst_color_factor = BlendingFactor::ZERO,
      src_alpha_factor = BlendingFactor::ONE,
      dst_alpha_factor = BlendingFactor::ZERO;

    enum class BlendingOperation
    {
        ADD,
        SUBTRACT,
        REVERSE_SUBTRACT,
        MIN,
        MAX,
    } color_blend_op = BlendingOperation::ADD,
      alpha_blend_op = BlendingOperation::ADD;
};

struct VertexBufferBindingInfo
{
    uint32_t stride = 0; // todo: derive from buffer view

    enum class InputRate
    {
        PER_VERTEX,
        PER_INSTANCE
    } input_rate = InputRate::PER_VERTEX;
};

struct VertexAttribute
{
    uint32_t binding_index;
    uint32_t offset;
    GpuDataFormat source_format;
};

struct PipelineCreateInfo
{
    struct VertexInput
    {
        std::vector<VertexBufferBindingInfo> bindings;
        std::vector<VertexAttribute> attributes;
    } vertex_input;

    std::shared_ptr<class SpirvShader> vertex_shader, fragment_shader;
    InputAssemblyState input_assembly;
    RasterizationState rasterization;
    DepthStencilState depth_stencil;
    ColorBlendState color_blend;

    struct AttachmentUsage
    {
        uint32_t native_format;
        GraphicsImageLayout initial_layout;
        GraphicsImageLayout final_layout;
    };

    std::vector<AttachmentUsage> attachment_usages;
};
}
