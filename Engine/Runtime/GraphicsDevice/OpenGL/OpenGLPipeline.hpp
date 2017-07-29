#pragma once

#include <GL/glew.h>
#include <map>

#include <Usagi/Engine/Runtime/GraphicsDevice/GDPipeline.hpp>

namespace yuki
{

/**
 * \brief 
 * GL_MAX_stage_COMPUTE_UNIFORM_BLOCKS is at least 14 for each shader stage, and
 * HLSL shaders do not have separate binding slots for each stage. In order to
 * uniquely identity binding slots for each shader stage, the following convention
 * is used:
 * - Each shader can use at most 14 uniform blocks
 * - Use monotonically increasing buffer binding indices
 * - Binding slots are assigned as such:
 *      Vertex shaders: 0-13
 *      Fragment shaders: 14-27
 * - The pipeline still index the slots for each stage starting from 0
 */
class OpenGLPipeline : public GDPipeline
{
    std::map<int, std::shared_ptr<class OpenGLVertexBuffer>> mVertexBuffers;
    std::map<int, std::shared_ptr<class OpenGLConstantBuffer>> mConstantBuffers;
    std::shared_ptr<class OpenGLVertexShader> mVertexShader;
    std::shared_ptr<class OpenGLFragmentShader> mFragmentShader;

    static constexpr size_t UBO_LIMIT = 14;
    static constexpr size_t VERTEX_SHADER_UBO_OFFSET = 0 * UBO_LIMIT;
    static constexpr size_t FRAGMENT_SHADER_UBO_OFFSET = 1 * UBO_LIMIT;

    GLuint mVertexArray = 0;
    /**
     * \brief We create a program object for each vertex and fragment shader combination
     * to enable cross-stage optimizations, which is unavailable when using program pipeline
     * object.
     */
    GLuint mProgram = 0;
    bool mProgramNeedCompilation = false;
    VertexOrder mVertexOrder = VertexOrder::COUNTER_CLOCKWISE;
    FaceCullingType mFaceCulling = FaceCullingType::NONE;
    bool mScissorTest = false;
    bool mDepthTest = true;
    bool mBlending = false;
    GLenum mBlendingColorOp = GL_FUNC_ADD, mBlendingAlphaOp = GL_FUNC_ADD;
    GLenum mBlendingSrcRgb = GL_ONE, mBlendingDestRgb = GL_ZERO, mBlendingSrcAlpha = GL_ONE, mBlendingDestAlpha = GL_ZERO;

    static GLenum _translateBlendingFactor(BlendingFactor factor);
    static GLenum _translateBlendingOperation(BlendingOperation op);

    void _setupCulling();
    /**
     * \brief Vertex post-processing, primitive assembly, and rasterization.
     */
    void _setupFragmentPreparation();
    void _setupPerFragmentOperations();
    void _setupColorBlending();
    void _setupShaderProgram();
    void _setupShaderBuffers();

    void _bindConstantBuffer(size_t slot, std::shared_ptr<yuki::ConstantBuffer> buffer);

public:
    OpenGLPipeline();
    void _deleteProgram();
    ~OpenGLPipeline() override;

    void vpSetVertexInputFormat(std::initializer_list<VertexAttributeSource> sources) override;
    void vpUseIndexBuffer(std::shared_ptr<IndexBuffer> buffer) override;
    void vpBindVertexBuffer(size_t slot, std::shared_ptr<VertexBuffer> buffer) override;
    void vsBindConstantBuffer(size_t slot, std::shared_ptr<ConstantBuffer> buffer) override;
    void vsUseVertexShader(std::shared_ptr<VertexShader> shader) override;

    void rsSetVertexOrder(VertexOrder order) override;
    void rsSetFaceCulling(FaceCullingType type) override;

    void fsBindConstantBuffer(size_t slot, std::shared_ptr<ConstantBuffer> buffer) override;
    void fsUseFragmentShader(std::shared_ptr<FragmentShader> shader) override;
    void fsEnableScissor(bool enable) override;
    void fsEnableDepthTest(bool enable) override;

    void bldEnableBlend(bool enable) override;
    void bldSetColorOp(BlendingOperation op) override;
    void bldSetAlphaOp(BlendingOperation op) override;
    void bldSetColorSrcFactor(BlendingFactor factor) override;
    void bldSetAlphaSrcFactor(BlendingFactor factor) override;
    void bldSetColorDestFactor(BlendingFactor factor) override;
    void bldSetAlphaDestFactor(BlendingFactor factor) override;

    /**
     * \brief
     * todo: clean up?
     */
    void assemble() override;
};

}
