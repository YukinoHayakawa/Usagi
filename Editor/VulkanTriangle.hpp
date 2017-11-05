#pragma once

#include <Usagi/Engine/Runtime/Graphics/Renderable.hpp>

namespace yuki
{

namespace graphics
{

class Pipeline;
class Buffer;

}

class VulkanTriangle : public graphics::Renderable
{
    struct graphics::Environment *mEnv;

    std::unique_ptr<graphics::Pipeline> mPipeline;
    struct VertexData
    {
        float   x, y, z, w;
        float   r, g, b, a;
    };
    std::unique_ptr<graphics::Buffer> mVertexBuffer;
    float mCounter1 = 0.f;

public:
    explicit VulkanTriangle(graphics::Environment *env);
    ~VulkanTriangle();

    void update(double delta_time) override;
    void populateCommandList(graphics::CommandList *command_list) override;
};

}
