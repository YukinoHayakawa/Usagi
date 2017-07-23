#include <stdexcept>

#include "OpenGLBlendState.hpp"
#include "OpenGLCommon.hpp"

GLenum yuki::OpenGLBlendState::_resolveFactor(Factor factor)
{
    switch(factor)
    {
        case Factor::ZERO: return GL_ZERO;
        case Factor::ONE: return GL_ONE;
        case Factor::SOURCE_ALPHA: return GL_SRC_ALPHA;
        case Factor::INV_SOURCE_ALPHA: return GL_ONE_MINUS_SRC_ALPHA;
        default: throw std::logic_error("unimplemented blending factor");
    }
}

yuki::OpenGLBlendState & yuki::OpenGLBlendState::enableBlend(bool enable)
{
    mEnable = enable;
    return *this;
}

GLenum yuki::OpenGLBlendState::_resolveOp(yuki::BlendState::Operation op)
{
    switch(op)
    {
        case Operation::ADD: return GL_FUNC_ADD;
        default: throw std::logic_error("unimplemented blending function");
    }
}

yuki::OpenGLBlendState & yuki::OpenGLBlendState::setColorOp(Operation op)
{
    mColorOp = _resolveOp(op);
    return *this;
}

yuki::OpenGLBlendState & yuki::OpenGLBlendState::setAlphaOp(Operation op)
{
    mAlphaOp = _resolveOp(op);
    return *this;
}

yuki::OpenGLBlendState & yuki::OpenGLBlendState::setColorSrcFactor(Factor factor)
{
    mSrcRgb = _resolveFactor(factor);
    return *this;
}

yuki::OpenGLBlendState & yuki::OpenGLBlendState::setAlphaSrcFactor(Factor factor)
{
    mSrcAlpha = _resolveFactor(factor);
    return *this;
}

yuki::OpenGLBlendState & yuki::OpenGLBlendState::setColorDestFactor(Factor factor)
{
    mDestRgb = _resolveFactor(factor);
    return *this;
}

yuki::OpenGLBlendState & yuki::OpenGLBlendState::setAlphaDestFactor(Factor factor)
{
    mDestAlpha = _resolveFactor(factor);
    return *this;
}

void yuki::OpenGLBlendState::use()
{
	if(mEnable)
	{
		glEnable(GL_BLEND);
		glBlendEquationSeparate(mColorOp, mAlphaOp);
		glBlendFuncSeparate(mSrcRgb, mDestRgb, mSrcAlpha, mDestAlpha);
	}
	else
	{
		glDisable(GL_BLEND);
	}
    YUKI_OPENGL_CHECK();
}
