#pragma once

#include <Usagi/Engine/Runtime/Graphics/Enum/CompareOp.hpp>

namespace usagi
{
enum class PrimitiveTopology
{
    POINT_LIST,
    LINE_LIST,
    LINE_STRIP,
    TRIANGLE_LIST,
    TRIANGLE_STRIP,
    TRIANGLE_FAN,
};

struct InputAssemblyState
{
    PrimitiveTopology topology = PrimitiveTopology::TRIANGLE_LIST;
};

enum class PolygonMode
{
    FILL,
    LINE,
    POINT
};

enum class FaceCullingMode
{
    NONE,
    FRONT,
    BACK
};

enum class FrontFace
{
    CLOCKWISE,
    COUNTER_CLOCKWISE
};

struct RasterizationState
{
    PolygonMode polygon_mode = PolygonMode::FILL;
    FaceCullingMode face_culling_mode = FaceCullingMode::BACK;
    FrontFace front_face = FrontFace::COUNTER_CLOCKWISE;

    // line width?
};

struct DepthStencilState
{
    bool depth_test_enable = true;
    bool depth_write_enable = true;
    CompareOp depth_compare_op = CompareOp::LESS;
    // todo stencil test
};

enum class BlendingFactor
{
    ZERO,
    ONE,
    SOURCE_ALPHA,
    ONE_MINUS_SOURCE_ALPHA,
};

enum class BlendingOperation
{
    ADD,
    SUBTRACT,
    REVERSE_SUBTRACT,
    MIN,
    MAX,
};

struct ColorBlendState
{
    bool enable = false;

    BlendingFactor src_color_factor = BlendingFactor::ONE,
        dst_color_factor = BlendingFactor::ZERO,
        src_alpha_factor = BlendingFactor::ONE,
        dst_alpha_factor = BlendingFactor::ZERO;

    BlendingOperation color_blend_op = BlendingOperation::ADD,
        alpha_blend_op = BlendingOperation::ADD;
};

enum class VertexInputRate
{
	PER_VERTEX,
	PER_INSTANCE,
};
}
