#include <stdexcept>

#include "OpenGLVertexBuffer.hpp"
#include "OpenGLVertexShader.hpp"
#include "OpenGLFragmentShader.hpp"
#include "OpenGLPipeline.hpp"
#include "OpenGLCommon.hpp"
#include <cassert>
#include <vector>

yuki::OpenGLPipeline::OpenGLPipeline()
{
    glCreateVertexArrays(1, &mVertexArray);
}

void yuki::OpenGLPipeline::_deleteProgram()
{
    if(mProgram) glDeleteProgram(mProgram);
    mProgram = 0;
}

yuki::OpenGLPipeline::~OpenGLPipeline()
{
    glDeleteVertexArrays(1, &mVertexArray);
    _deleteProgram();
}

void yuki::OpenGLPipeline::vpSetVertexInputFormat(std::initializer_list<VertexAttributeSource> sources)
{
    for(auto i : sources)
    {
        glVertexArrayAttribFormat(
            mVertexArray,
            i.index,
            i.count,
            OpenGLTranslateNativeDataType(i.type),
            i.normalize,
            i.offset
        );
        glVertexArrayAttribBinding(
            mVertexArray,
            i.index,
            i.buffer_index
        );
        YUKI_OPENGL_CHECK();
    }
}

void yuki::OpenGLPipeline::vpUseIndexBuffer(std::shared_ptr<IndexBuffer> buffer)
{
    // todo
}

void yuki::OpenGLPipeline::vpBindVertexBuffer(size_t slot, std::shared_ptr<VertexBuffer> buffer)
{
    auto real_buffer = std::dynamic_pointer_cast<OpenGLVertexBuffer>(buffer);

    glVertexArrayVertexBuffer(
        mVertexArray,
        slot,
        real_buffer->_getBufferName(),
        0,
        real_buffer->getElementSize()
    );
    YUKI_OPENGL_CHECK();

    mVertexBuffers[slot] = std::move(real_buffer);
}

void yuki::OpenGLPipeline::vsBindConstantBuffer(size_t slot, std::shared_ptr<ConstantBuffer> buffer)
{
}

void yuki::OpenGLPipeline::vsUseVertexShader(std::shared_ptr<VertexShader> shader)
{
    mVertexShader = std::dynamic_pointer_cast<OpenGLVertexShader>(shader);
    if(!mVertexShader) throw std::bad_cast();
    mProgramNeedCompilation = true;
}

void yuki::OpenGLPipeline::rsSetVertexOrder(VertexOrder order)
{
    mVertexOrder = order;
}

void yuki::OpenGLPipeline::rsSetFaceCulling(FaceCullingType type)
{
    mFaceCulling = type;
}

void yuki::OpenGLPipeline::fsBindConstantBuffer(size_t slot, std::shared_ptr<ConstantBuffer> buffer)
{
}

void yuki::OpenGLPipeline::fsUseFragmentShader(std::shared_ptr<FragmentShader> shader)
{
    mFragmentShader = std::dynamic_pointer_cast<OpenGLFragmentShader>(shader);
    if(!mFragmentShader) throw std::bad_cast();
    mProgramNeedCompilation = true;
}

void yuki::OpenGLPipeline::fsEnableScissor(bool enable)
{
    mScissorTest = enable;
}

void yuki::OpenGLPipeline::fsEnableDepthTest(bool enable)
{
    mDepthTest = enable;
}

void yuki::OpenGLPipeline::bldEnableBlend(bool enable)
{
    mBlending = enable;
}

void yuki::OpenGLPipeline::bldSetColorOp(BlendingOperation op)
{
    mBlendingColorOp = _translateBlendingOperation(op);
}

void yuki::OpenGLPipeline::bldSetAlphaOp(BlendingOperation op)
{
    mBlendingAlphaOp = _translateBlendingOperation(op);
}

void yuki::OpenGLPipeline::bldSetColorSrcFactor(BlendingFactor factor)
{
    mBlendingSrcRgb = _translateBlendingFactor(factor);
}

void yuki::OpenGLPipeline::bldSetAlphaSrcFactor(BlendingFactor factor)
{
    mBlendingSrcAlpha = _translateBlendingFactor(factor);
}

void yuki::OpenGLPipeline::bldSetColorDestFactor(BlendingFactor factor)
{
    mBlendingDestRgb = _translateBlendingFactor(factor);
}

void yuki::OpenGLPipeline::bldSetAlphaDestFactor(BlendingFactor factor)
{
    mBlendingDestAlpha = _translateBlendingFactor(factor);
}

GLenum yuki::OpenGLPipeline::_translateBlendingFactor(BlendingFactor factor)
{
    switch(factor)
    {
        case BlendingFactor::ZERO: return GL_ZERO;
        case BlendingFactor::ONE: return GL_ONE;
        case BlendingFactor::SOURCE_ALPHA: return GL_SRC_ALPHA;
        case BlendingFactor::INV_SOURCE_ALPHA: return GL_ONE_MINUS_SRC_ALPHA;
        default: throw std::logic_error("unimplemented blending factor");
    }
}

GLenum yuki::OpenGLPipeline::_translateBlendingOperation(BlendingOperation op)
{
    switch(op)
    {
        case BlendingOperation::ADD: return GL_FUNC_ADD;
        case BlendingOperation::SUBTRACT: return GL_FUNC_SUBTRACT;
        case BlendingOperation::REVERSE_SUBTRACT: return GL_FUNC_REVERSE_SUBTRACT;
        case BlendingOperation::MIN: return GL_MIN;
        case BlendingOperation::MAX: return GL_MAX;
        default: throw std::logic_error("unimplemented blending function");
    }
}

void yuki::OpenGLPipeline::_setupCulling()
{
    switch(mFaceCulling)
    {
        case FaceCullingType::NONE: glDisable(GL_CULL_FACE);
            break;
        case FaceCullingType::FRONT:
        {
            glEnable(GL_CULL_FACE);
            switch(mVertexOrder)
            {
                case VertexOrder::COUNTER_CLOCKWISE: glFrontFace(GL_CCW);
                    break;
                case VertexOrder::CLOCKWISE: glFrontFace(GL_CW);
                    break;
                default: throw std::logic_error("invalid vertex order");
            }
            break;
        }
        case FaceCullingType::BACK:
        {
            glEnable(GL_CULL_FACE);
            switch(mVertexOrder)
            {
                case VertexOrder::COUNTER_CLOCKWISE: glFrontFace(GL_CW);
                    break;
                case VertexOrder::CLOCKWISE: glFrontFace(GL_CCW);
                    break;
                default: throw std::logic_error("invalid vertex order");
            }
            break;
        }
        default: throw std::logic_error("invalid face culling type");
    }
}

void yuki::OpenGLPipeline::_setupPerFragmentOperations()
{
    mScissorTest ? glEnable(GL_SCISSOR_TEST) : glDisable(GL_SCISSOR_TEST);
    mDepthTest ? glEnable(GL_DEPTH_TEST) : glDisable(GL_DEPTH_TEST);
}

void yuki::OpenGLPipeline::_setupFragmentPreparation()
{
    _setupCulling();
}

void yuki::OpenGLPipeline::_setupColorBlending()
{
    if(mBlending)
    {
        glEnable(GL_BLEND);
        glBlendEquationSeparate(mBlendingColorOp, mBlendingAlphaOp);
        glBlendFuncSeparate(mBlendingSrcRgb, mBlendingDestRgb, mBlendingSrcAlpha, mBlendingDestAlpha);
    }
    else
    {
        glDisable(GL_BLEND);
    }
    YUKI_OPENGL_CHECK();
}

void yuki::OpenGLPipeline::_setupShaderProgram()
{
    if(mProgramNeedCompilation)
    {
        assert(mVertexShader);
        assert(mFragmentShader);

        _deleteProgram();
        mProgram = glCreateProgram();
        
        mVertexShader->_attachToProgram(mProgram);
        mFragmentShader->_attachToProgram(mProgram);
        glLinkProgram(mProgram);
        mVertexShader->_detachFromProgram(mProgram);
        mFragmentShader->_detachFromProgram(mProgram);

        GLint result;
        int info_log_length;

        // Check the program
        glGetProgramiv(mProgram, GL_LINK_STATUS, &result);
        glGetProgramiv(mProgram, GL_INFO_LOG_LENGTH, &info_log_length);
        if(info_log_length > 0)
        {
            std::vector<char> ProgramErrorMessage(info_log_length + 1);
            glGetProgramInfoLog(mProgram, info_log_length, nullptr, &ProgramErrorMessage[0]);
            printf("%s\n", &ProgramErrorMessage[0]);
        }
        YUKI_OPENGL_CHECK();

        // if we own the shader objects, free them to save memory.
        if(mVertexShader.use_count() == 1)
            mVertexShader.reset();
        if(mFragmentShader.use_count() == 1)
            mFragmentShader.reset();

        mProgramNeedCompilation = false;
    }
    assert(mProgram);
    glUseProgram(mProgram);
}

void yuki::OpenGLPipeline::_setupShaderBuffers()
{
    glBindVertexArray(mVertexArray);
    // todo bind uniform buffers
}

void yuki::OpenGLPipeline::assemble()
{
    _setupShaderProgram();
    _setupShaderBuffers();

    // todo bind resources (textures, samplers)

    // this is required to set point size from the shader
    glEnable(GL_PROGRAM_POINT_SIZE);

    _setupFragmentPreparation();
    _setupPerFragmentOperations();

    _setupColorBlending();

    YUKI_OPENGL_CHECK();

    // todo: unbind program & VAO
}
