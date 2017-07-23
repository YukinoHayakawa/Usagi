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

    BlendState & setOp(Operation op)
    {
        setColorOp(op);
        setAlphaOp(op);
        return *this;
    }

    BlendState & setSrcFactor(Factor factor)
    {
        setColorSrcFactor(factor);
        setAlphaSrcFactor(factor);
        return *this;
    }

    BlendState & setDestFactor(Factor factor)
    {
        setColorDestFactor(factor);
        setAlphaDestFactor(factor);
        return *this;
    }

    virtual BlendState & setColorOp(Operation op) = 0;
    virtual BlendState & setAlphaOp(Operation op) = 0;
    virtual BlendState & setColorSrcFactor(Factor factor) = 0;
    virtual BlendState & setAlphaSrcFactor(Factor factor) = 0;
    virtual BlendState & setColorDestFactor(Factor factor) = 0;
    virtual BlendState & setAlphaDestFactor(Factor factor) = 0;

    virtual void use() = 0;
};

}
