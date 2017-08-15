#pragma once

#include <memory>

namespace yuki
{

/**
 * \brief Issue drawing commands to a GraphicsDevice.
 * Creator of the instance should pass the GraphicsDevice to the
 * constructor of the derived class, in which the class creates
 * necessary resources.
 * 
 * todo: allow runtime re-initialization, for reloading resources?
 */
class Renderable
{
protected:
    std::shared_ptr<class GraphicsDevice> mGraphicsDevice;

public:
    Renderable(std::shared_ptr<GraphicsDevice> graphics_device);
    virtual ~Renderable() = default;

    /**
     * \brief Setup rendering pipeline and issue rendering commands.
     */
    virtual void render(const class Clock &render_clock) = 0;
};

}
