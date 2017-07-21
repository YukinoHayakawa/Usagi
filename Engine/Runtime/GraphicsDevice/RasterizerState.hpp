#pragma once

namespace yuki
{

class RasterizerState
{
public:
    enum class FaceCullingType
    {
        NONE, FRONT, BACK
    };

    enum class VertexOrder
    {
        COUNTER_CLOCKWISE, CLOCKWISE
    };

    virtual ~RasterizerState() = default;

    // default: disabled
    virtual RasterizerState & enableScissor(bool enable) = 0;
    // default: enabled
    virtual RasterizerState & enableDepthTest(bool enable) = 0;
    // default: counter-clockwise
    virtual RasterizerState & setVertexOrder(VertexOrder order) = 0;
    // default: disabled
    virtual RasterizerState & setFaceCulling(FaceCullingType type) = 0;

    /**
     * \brief Apply current config to the rendering pipeline, which is
     * associated with the GraphicsDevice creating this instance.
     * The effect may remain after drawing calls are issued. To ensure
     * a correct setup is used, call this method each time before issuing
     * tasks to the pipeline.
     */
    virtual void use() = 0;
};

}
