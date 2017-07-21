#pragma once

namespace yuki
{

class BlendState
{
public:
    virtual ~BlendState() = default;

    // default: disabled
    virtual BlendState & enableBlend(bool enable) = 0;

    enum class Factor
    {
        ZERO, ONE,
        SOURCE_ALPHA, INV_SOURCE_ALPHA,
    };

    enum class Operation
    {
        ADD
    };

    virtual BlendState & setColorOp(Operation op) = 0;
    virtual BlendState & setAlphaOp(Operation op) = 0;
    virtual BlendState & setColorSrcFactor(Factor factor) = 0;
    virtual BlendState & setAlphaSrcFactor(Factor factor) = 0;
    virtual BlendState & setColorDestFactor(Factor factor) = 0;
    virtual BlendState & setAlphaDestFactor(Factor factor) = 0;

    virtual void use() = 0;
};

}
