﻿#pragma once

#include <Usagi/Engine/Utility/Noncopyable.hpp>

#include "PipelineCreateInfo.hpp"

namespace usagi
{
class SpirvBinary;
class GraphicsPipeline;

/**
 * \brief Compiles graphics pipeline from state specifications. Default states
 * are defined in corresponding state structs. Resource binding info is
 * generated by reflecting the shaders.
 */
class GraphicsPipelineCompiler : Noncopyable
{
public:
    virtual ~GraphicsPipelineCompiler() = default;

    virtual void setVertexShader(SpirvBinary *shader) = 0;
    virtual void setFragmentShader(SpirvBinary *shader) = 0;

    virtual void setVertexBufferBinding(
        uint32_t binding_index,
        VertexBufferBindingInfo &info) = 0;
    virtual void setVertexAttribute(
        std::string attr_name,
        const VertexAttribute &info) = 0;
    virtual void setVertexAttribute(
        uint32_t attr_location,
        const VertexAttribute &info) = 0;

    virtual void setInputAssemblyState(const InputAssemblyState &state) = 0;
    virtual void setRasterizationState(const RasterizationState &state) = 0;
    virtual void setDepthStencilState(const DepthStencilState &state) = 0;
    virtual void setColorBlendState(const ColorBlendState &state) = 0;

    virtual std::shared_ptr<GraphicsPipeline> compile() = 0;
};
}
