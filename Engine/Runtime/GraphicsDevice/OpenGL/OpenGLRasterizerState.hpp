#pragma once

#include <Usagi/Engine/Runtime/GraphicsDevice/RasterizerState.hpp>

namespace yuki
{

class OpenGLRasterizerState : public RasterizerState
{
    bool mScissor = false, mDepth = false;
    FaceCullingType mFaceCulling = FaceCullingType::NONE;
    VertexOrder mVertexOrder = VertexOrder::COUNTER_CLOCKWISE;

public:
    OpenGLRasterizerState & enableScissor(bool enable) override;
    OpenGLRasterizerState & enableDepthTest(bool enable) override;
    OpenGLRasterizerState & setVertexOrder(VertexOrder order) override;
    OpenGLRasterizerState & setFaceCulling(FaceCullingType type) override;

    void use() override;
};

}
