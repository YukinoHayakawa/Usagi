#include "Renderable.hpp"

yuki::Renderable::Renderable(std::shared_ptr<GraphicsDevice> graphics_device)
    : mGraphicsDevice { std::move(graphics_device) }
{
}
