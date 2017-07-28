#pragma once

#include <GL/glew.h>
#include <map>

#include <Usagi/Engine/Runtime/GraphicsDevice/GDPipeline.hpp>

namespace yuki
{

class OpenGLVertexBuffer;
class OpenGLVertexShader;
class OpenGLFragmentShader;

/**
 * \brief 
 */
class OpenGLPipeline : public GDPipeline
{
    std::map<size_t, std::shared_ptr<OpenGLVertexBuffer>> mVertexBuffers;
    std::shared_ptr<OpenGLVertexShader> mVertexShader;
    std::shared_ptr<OpenGLFragmentShader> mFragmentShader;

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
