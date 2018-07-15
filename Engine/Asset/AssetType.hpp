#pragma once

namespace usagi
{
enum class AssetType
{
    UNKNOWN,
    PACKAGE,
    // contains sub-assets
    DIRECTORY,
    // compiled SPIR-V
    VERTEX_SHADER,
    // compiled SPIR-V
    FRAGMENT_SHADER,
    // GLSL
    VERTEX_SHADER_SOURCE,
    // GLSL
    FRAGMENT_SHADER_SOURCE,
    TEXTURE,
};
}
