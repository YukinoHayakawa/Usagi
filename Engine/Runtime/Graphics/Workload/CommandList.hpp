#pragma once

#include <vector>

#include <Usagi/Engine/Utility/Noncopyable.hpp>

namespace yuki::graphics
{

enum class ImageLayout;

struct PipelineAssembly
{
    class Pipeline *pipeline;
    // the number of render target attachments should match that defined in the pipeline
    std::vector<class Image*> attachments;
};

// todo provide higher level abstraction of commands e.g. render pass to ensure that pipeline is bound
class CommandList : Noncopyable
{
public:
    virtual ~CommandList() = default;

    virtual void begin() = 0;

    //virtual void imageTransitionFromPresentToDraw(class GraphicsImage *image) = 0;
    //virtual void imageTransitionFromDrawToPresent(class GraphicsImage *image) = 0;

    virtual void bindPipeline(class Pipeline * pipeline) = 0;

    // viewport and scissor must be set in each command list at least once
    virtual void setViewport(float x, float y, float width, float height) = 0;
    virtual void setScissor(int32_t x, int32_t y, int32_t width, int32_t height) = 0;

    virtual void bindVertexBuffer(uint32_t slot, class Buffer *buffer) = 0;

    virtual void draw(uint32_t vertex_count, uint32_t instance_count, uint32_t first_vertex, uint32_t first_instance) = 0;

    virtual void end() = 0;
};

}
