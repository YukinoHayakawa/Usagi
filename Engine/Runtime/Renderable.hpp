#pragma once

namespace yuki
{

class Clock;
class GraphicsDevice;

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
public:
    virtual ~Renderable() = default;

    /**
     * \brief Setup rendering pipeline and issue rendering commands.
     * \param gd GraphicsDevice instance.
     */
    virtual void render(GraphicsDevice &gd, const Clock &render_clock) = 0;
};

}
