#pragma once

namespace usagi
{
enum class GraphicsPipelineStage
{
    TOP_OF_PIPE,
    VERTEX_INPUT,
    VERTEX_SHADER,
    GEOMETRY_SHADER,
    FRAGMENT_SHADER,
    EARLY_FRAGMENT_TESTS,
    LATE_FRAGMENT_TESTS,
    COLOR_ATTACHMENT_OUTPUT,
    TRANSFER,
    BOTTOM_OF_PIPE,
    HOST,
};
}
