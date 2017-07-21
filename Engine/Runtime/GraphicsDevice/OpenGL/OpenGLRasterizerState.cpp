#include <stdexcept>
#include <GL/glew.h>

#include "OpenGLRasterizerState.hpp"

yuki::OpenGLRasterizerState & yuki::OpenGLRasterizerState::enableScissor(bool enable)
{
    mScissor = enable;
    return *this;
}

yuki::OpenGLRasterizerState & yuki::OpenGLRasterizerState::enableDepthTest(bool enable)
{
    mDepth = enable;
    return *this;
}

yuki::OpenGLRasterizerState & yuki::OpenGLRasterizerState::setVertexOrder(VertexOrder order)
{
    mVertexOrder = order;
    return *this;
}

yuki::OpenGLRasterizerState & yuki::OpenGLRasterizerState::setFaceCulling(FaceCullingType type)
{
    mFaceCulling = type;
    return *this;
}

void yuki::OpenGLRasterizerState::use()
{
    mScissor ? glEnable(GL_SCISSOR_TEST) : glDisable(GL_SCISSOR_TEST);
    mDepth ? glEnable(GL_DEPTH_TEST) : glDisable(GL_DEPTH_TEST);

    switch(mFaceCulling)
    {
        case FaceCullingType::NONE: glDisable(GL_CULL_FACE); break;
        case FaceCullingType::FRONT:
        {
            glEnable(GL_CULL_FACE);
            switch(mVertexOrder)
            {
                case VertexOrder::COUNTER_CLOCKWISE: glFrontFace(GL_CCW); break;
                case VertexOrder::CLOCKWISE: glFrontFace(GL_CW);  break;
                default: throw std::logic_error("invalid vertex order");
            }
            break;
        }
        case FaceCullingType::BACK:
        {
            glEnable(GL_CULL_FACE);
            switch(mVertexOrder)
            {
                case VertexOrder::COUNTER_CLOCKWISE: glFrontFace(GL_CW); break;
                case VertexOrder::CLOCKWISE: glFrontFace(GL_CCW);  break;
                default: throw std::logic_error("invalid vertex order");
            }
            break;
        }
        default: throw std::logic_error("invalid face culling type");
    }

    // this is required to set point size in the shader, not sure
    // whether dx has an equivalent to this.
    glEnable(GL_PROGRAM_POINT_SIZE);
}
