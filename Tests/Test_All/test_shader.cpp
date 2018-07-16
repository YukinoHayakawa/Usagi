#include <gtest/gtest.h>
#include <Usagi/Engine/Graphics/Shader/SpirvBinary.hpp>

using namespace usagi;

TEST(ShaderTest, VertexShaderCompilationTest)
{
    EXPECT_NO_THROW(SpirvBinary::fromGlslSourceFile(
        "data/shaders/glsl_shader.vert", ShaderStage::VERTEX
    ));
}

TEST(ShaderTest, FragmentShaderCompilationTest)
{
    EXPECT_NO_THROW(SpirvBinary::fromGlslSourceFile(
        "data/shaders/glsl_shader.frag", ShaderStage::FRAGMENT
    ));
}

TEST(ShaderTest, VertexShaderLoadTest)
{
    EXPECT_NO_THROW(SpirvBinary::fromFile(
        "data/shaders/vert.spv"
    ));
}

TEST(ShaderTest, FragmentShaderLoadTest)
{
    EXPECT_NO_THROW(SpirvBinary::fromFile(
        "data/shaders/frag.spv"
    ));
}
