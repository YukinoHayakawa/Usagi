#pragma once

#include <vector>
#include <memory>
#include <functional>

#include <Usagi/Engine/Game/Subsystem.hpp>
#include <chrono>

namespace usagi
{
class Framebuffer;
class GraphicsCommandList;

/**
 * \brief Subsystem that uses GPU to draw.
 */
class RenderableSubsystem : public Subsystem
{
    void update(const TimeDuration &dt) override final
    {
        throw std::runtime_error(
            "Framebuffer must be provided to update RenderableSubsystem.");
    }

public:
    /**
     * \brief 
     * \param dt 
     * \param framebuffer 
     * \param cmd_out Command list receiver, should be thread-safe.
     */
    virtual void update(
        const TimeDuration &dt,
        Framebuffer *framebuffer,
        const std::function<void(std::shared_ptr<GraphicsCommandList>)> &cmd_out
    ) = 0;
};
}
 