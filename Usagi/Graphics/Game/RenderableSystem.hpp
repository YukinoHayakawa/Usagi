#pragma once

#include <memory>

#include <Usagi/Game/System.hpp>

namespace usagi
{
class RenderTargetDescriptor;
class RenderTarget;
class Framebuffer;
class GraphicsCommandList;

/**
 * \brief System that uses GPU to draw.
 */
class RenderableSystem : virtual public System
{
protected:
    std::shared_ptr<RenderTarget> mRenderTarget;

public:
    virtual void createRenderTarget(RenderTargetDescriptor &descriptor) = 0;

    /**
     * \brief Create pipelines. Apart from during initialization, may also be
     * invoked when rendering setting is changed.
     * The subsystem should use the RenderTargetProvider to declare required
     * render attachments.
     */
    virtual void createPipelines() = 0;

    /**
     * \brief Record command lists. In order to achieve multi-threaded
     * recording, this method may not modify the game states. Any modification
     * to the game states should happen in update().
     * Secondary command lists can be used if the subsystem internally records
     * the command list in parallel.
     * \param clock
     */
    virtual std::shared_ptr<GraphicsCommandList> render(const Clock &clock) = 0;
};
}
