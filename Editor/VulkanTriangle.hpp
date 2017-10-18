#pragma once

#include <Usagi/Engine/Runtime/Graphics/Renderable3.hpp>

namespace yuki
{

class VulkanTriangle : public Renderable3
{
    struct GraphicsEnvironment *mEnv;

    std::unique_ptr<class GraphicsPipeline> mPipeline;
    struct VertexData
    {
        float   x, y, z, w;
        float   r, g, b, a;
    };
    std::unique_ptr<class GraphicsBuffer> mVertexBuffer;
    float mCounter1 = 0.f;

public:
    explicit VulkanTriangle(GraphicsEnvironment *env);
    ~VulkanTriangle();

    void update(double delta_time) override;
    void populateCommandList(GraphicsCommandList *command_list) override;
};

}
