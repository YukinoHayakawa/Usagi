#pragma once

#include <memory>
#include <vector>

#include "../Resource/ImageLayout.hpp"
#include "../Resource/DataFormat.hpp"

namespace yuki::graphics
{

struct InputAssembleState
{
    enum class PrimitiveTopology
    {
        POINT_LIST,
        LINE_LIST, LINE_STRIP,
        TRIANGLE_LIST, TRIANGLE_STRIP, TRIANGLE_FAN,
    } topology;
};

struct RasterizationState
{
    enum class PolygonMode
    {
        FILL, LINE, POINT
    } polygon_mode = PolygonMode::FILL;

    enum class FaceCullingMode
    {
        NONE, FRONT, BACK
    } face_culling_mode = FaceCullingMode::BACK;

    enum class FrontFace
    {
        CLOCKWISE, COUNTER_CLOCKWISE
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
        ZERO, ONE,
        SOURCE_ALPHA, ONE_MINUS_SOURCE_ALPHA,
    }   src_color_factor = BlendingFactor::ONE,
        dst_color_factor = BlendingFactor::ZERO,
        src_alpha_factor = BlendingFactor::ONE,
        dst_alpha_factor = BlendingFactor::ZERO;

    enum class BlendingOperation
    {
        ADD, SUBTRACT, REVERSE_SUBTRACT, MIN, MAX,
    }   color_blend_op = BlendingOperation::ADD,
        alpha_blend_op = BlendingOperation::ADD;
};

struct VertexBufferBinding
{
    uint32_t binding_slot;
    uint32_t stride; // todo derive from buffer view

    enum class InputRate
    {
        PER_VERTEX, PER_INSTANCE
    } input_rate = InputRate::PER_VERTEX;
};

struct VertexAttribute
{
    uint32_t location;
    uint32_t binding_slot;
    DataFormat source_format;
    uint32_t offset;
};

struct PipelineCreateInfo
{
    struct VertexInput
    {
        std::vector<VertexBufferBinding> bindings;
        std::vector<VertexAttribute> attributes;
    } vertex_input;

    std::shared_ptr<class SpirvShader> vertex_shader, fragment_shader;
    InputAssembleState input_assembly;
    RasterizationState rasterization;
    DepthStencilState depth_stencil;
    ColorBlendState color_blend;

    struct AttachmentUsage
    {
        uint32_t native_format;
        ImageLayout initial_layout;
        ImageLayout final_layout;
    };
    std::vector<AttachmentUsage> attachment_usages;
};

}
