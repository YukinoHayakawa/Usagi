#pragma once

#include <memory>
#include <functional>

#include <Usagi/Game/Subsystem.hpp>

namespace usagi
{
class Framebuffer;
class GraphicsCommandList;

/**
 * \brief Subsystem that uses GPU to draw.
 */
class RenderableSubsystem : virtual public Subsystem
{
public:
    using CommandListSink =
        std::function<void(std::shared_ptr<GraphicsCommandList>)>;

    /**
     * \brief Record command lists. In order to achieve multi-threaded
     * recording, this method may not modify the game states. Any modification
     * to the game states should happen in update().
     * \param clock
     * \param framebuffer
     * \param cmd_out Command list receiver, should be thread-safe.
     */
    virtual void render(
        const Clock &clock,
        std::shared_ptr<Framebuffer> framebuffer,
        const CommandListSink &cmd_out
    ) const = 0;
};
}
