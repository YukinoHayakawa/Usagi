#pragma once

#include <GL/glew.h>

#include <Usagi/Engine/Runtime/GraphicsDevice/BlendState.hpp>

namespace yuki
{

class OpenGLBlendState : public BlendState
{
    bool mEnable = false;
    GLenum mColorOp = GL_FUNC_ADD, mAlphaOp = GL_FUNC_ADD;
    GLenum mSrcRgb = GL_ONE, mDestRgb = GL_ZERO, mSrcAlpha = GL_ONE, mDestAlpha = GL_ZERO;

    static GLenum _resolveFactor(Factor factor);
    static GLenum _resolveOp(Operation op);

public:
    OpenGLBlendState & enableBlend(bool enable) override;
    OpenGLBlendState & setColorOp(Operation op) override;
    OpenGLBlendState & setAlphaOp(Operation op) override;
    OpenGLBlendState & setColorSrcFactor(Factor factor) override;
    OpenGLBlendState & setAlphaSrcFactor(Factor factor) override;
    OpenGLBlendState & setColorDestFactor(Factor factor) override;
    OpenGLBlendState & setAlphaDestFactor(Factor factor) override;

    void use() override;
};

}
