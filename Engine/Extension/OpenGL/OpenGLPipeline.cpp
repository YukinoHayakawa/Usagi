#include <cassert>
#include <vector>
#include <iostream>

#include <Usagi/Engine/Utility/TypeCast.hpp>
#include "OpenGLVertexBuffer.hpp"
#include "OpenGLVertexShader.hpp"
#include "OpenGLFragmentShader.hpp"
#include "OpenGLPipeline.hpp"
#include "detail/OpenGLCommon.hpp"
#include "OpenGLConstantBuffer.hpp"
#include "OpenGLSampler.hpp"
#include "OpenGLTexture.hpp"

namespace
{

/**
 * \brief Resolve the target pointer type from the map definition,
 * then cast the source pointer to the target type and store it in
 * the map by the slot number.
 * \tparam Map 
 * \tparam Source 
 * \param dest 
 * \param slot 
 * \param ptr 
 */
template <typename Map, typename Source>
void cast_and_store(Map &dest, int slot, const std::shared_ptr<Source> &ptr)
{
    typedef typename Map::value_type::second_type::element_type Target;
    auto real = yuki::dynamic_pointer_cast_throw<Target>(ptr);
    dest[slot] = std::move(real);
}

}

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

void yuki::OpenGLPipeline::vpSetVertexInputFormat(const std::vector<yuki::GDPipeline::VertexAttributeSource> &sources)
{
    for(auto i : sources)
    {
        glEnableVertexArrayAttrib(mVertexArray, i.index);
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

void yuki::OpenGLPipeline::vpUseIndexBuffer(const std::shared_ptr<yuki::IndexBuffer> &buffer)
{
    // todo
}

void yuki::OpenGLPipeline::vpBindVertexBuffer(size_t slot, const std::shared_ptr<yuki::VertexBuffer> &buffer)
{
    auto real_buffer = dynamic_pointer_cast_throw<OpenGLVertexBuffer>(buffer);

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

void yuki::OpenGLPipeline::_bindConstantBuffer(size_t binding_index, std::shared_ptr<yuki::ConstantBuffer> buffer)
{
    cast_and_store(mConstantBuffers, binding_index, buffer);
}

void yuki::OpenGLPipeline::vsBindConstantBuffer(size_t slot, const std::shared_ptr<yuki::ConstantBuffer> &buffer)
{
    assert(slot < UBO_LIMIT);
    _bindConstantBuffer(slot + VERTEX_SHADER_UBO_OFFSET, buffer);
}

void yuki::OpenGLPipeline::vsUseVertexShader(const std::shared_ptr<yuki::VertexShader> &shader)
{
    mVertexShader = dynamic_pointer_cast_throw<OpenGLVertexShader>(shader);
    mProgramNeedCompilation = true;
}

void yuki::OpenGLPipeline::vsBindTexture(size_t slot, const std::shared_ptr<yuki::GDTexture> &texture)
{
    mVertexTextureUnitManager.bindTexture(slot, texture);
}

void yuki::OpenGLPipeline::vsBindSampler(size_t slot, const std::shared_ptr<yuki::GDSampler> &sampler)
{
    mVertexTextureUnitManager.bindSampler(slot, sampler);
}

void yuki::OpenGLPipeline::vsSamplerUsageHint(const std::vector<yuki::GDPipeline::SamplerUsage> &usages)
{
    mVertexTextureUnitManager.samplerUsageHint(usages);
}

void yuki::OpenGLPipeline::rsSetVertexOrder(VertexOrder order)
{
    mVertexOrder = order;
}

void yuki::OpenGLPipeline::rsSetFaceCulling(FaceCullingType type)
{
    mFaceCulling = type;
}

void yuki::OpenGLPipeline::fsBindConstantBuffer(size_t slot, const std::shared_ptr<yuki::ConstantBuffer> &buffer)
{
    assert(slot < UBO_LIMIT);
    _bindConstantBuffer(slot + FRAGMENT_SHADER_UBO_OFFSET, buffer);
}

void yuki::OpenGLPipeline::fsUseFragmentShader(const std::shared_ptr<yuki::FragmentShader> &shader)
{
    mFragmentShader = dynamic_pointer_cast_throw<OpenGLFragmentShader>(shader);
    mProgramNeedCompilation = true;
}

void yuki::OpenGLPipeline::fsBindTexture(size_t slot, const std::shared_ptr<yuki::GDTexture> &texture)
{
    mFragmentTextureUnitManager.bindTexture(slot, texture);
}

void yuki::OpenGLPipeline::fsBindSampler(size_t slot, const std::shared_ptr<yuki::GDSampler> &sampler)
{
    mFragmentTextureUnitManager.bindSampler(slot, sampler);
}

void yuki::OpenGLPipeline::fsSamplerUsageHint(const std::vector<SamplerUsage> &usages)
{
    mFragmentTextureUnitManager.samplerUsageHint(usages);
}

void yuki::OpenGLPipeline::fdEnableScissor(bool enable)
{
    mScissorTest = enable;
}

void yuki::OpenGLPipeline::fdEnableDepthTest(bool enable)
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

void yuki::OpenGLPipeline::TextureUnitManager::bindTexture(size_t slot, const std::shared_ptr<GDTexture> &texture)
{
    cast_and_store(mTextures, slot, texture);
}

void yuki::OpenGLPipeline::TextureUnitManager::bindSampler(size_t slot, const std::shared_ptr<GDSampler> &sampler)
{
    cast_and_store(mSamplers, slot, sampler);
}

void yuki::OpenGLPipeline::TextureUnitManager::samplerUsageHint(const std::vector<SamplerUsage> &usages)
{
    mTextureUnitUsages = usages;
}

void yuki::OpenGLPipeline::TextureUnitManager::bindToTextureUnits()
{
    for(auto i = 0; i < mTextureUnitUsages.size(); ++i)
    {
        auto &u = mTextureUnitUsages[i];
        mSamplers[u.sampler_slot]->_bindToTextureUnit(i + mOffset);
        mTextures[u.tex_slot]->_bindToTextureUnit(i + mOffset);
    }
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
    YUKI_OPENGL_CHECK();
}

void yuki::OpenGLPipeline::_setupPerFragmentOperations()
{
    mScissorTest ? glEnable(GL_SCISSOR_TEST) : glDisable(GL_SCISSOR_TEST);
    mDepthTest ? glEnable(GL_DEPTH_TEST) : glDisable(GL_DEPTH_TEST);
    YUKI_OPENGL_CHECK();
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

        std::cout << "Linking program" << std::endl;
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
    YUKI_OPENGL_CHECK();
}

void yuki::OpenGLPipeline::_setupShaderBuffers()
{
    glBindVertexArray(mVertexArray);

    for(auto &&i : mConstantBuffers)
    {
        glBindBufferBase(GL_UNIFORM_BUFFER, i.first, i.second->_getBufferName());
    }
    // todo bind uniform buffers

    YUKI_OPENGL_CHECK();
}

void yuki::OpenGLPipeline::_setupTextureUnits()
{
    mVertexTextureUnitManager.bindToTextureUnits();
    mFragmentTextureUnitManager.bindToTextureUnits();
}

void yuki::OpenGLPipeline::assemble()
{
    // perform error checking in each method!

    _setupShaderProgram();
    _setupShaderBuffers();
    _setupTextureUnits();

    // todo bind resources (textures, samplers)

    // this is required to set point size from the shader
    glEnable(GL_PROGRAM_POINT_SIZE);

    _setupFragmentPreparation();
    _setupPerFragmentOperations();

    _setupColorBlending();

    // todo: unbind program & VAO
}
