#include "OverlayRenderingSystem.hpp"

#include <Usagi/Graphics/RenderWindow.hpp>
#include <Usagi/Runtime/Graphics/Swapchain.hpp>
#include <Usagi/Runtime/Window/Window.hpp>

void usagi::OverlayRenderingSystem::setSizeFunctionsFromRenderWindow(
    RenderWindow *render_window)
{
    setFrameBufferSizeFunc([=]() {
        // don't return result from cast directly, as it will be referencing
        // invalid vector from the stack
        return render_window->swapchain->size().cast<float>().eval();
    });
    setWindowSizeFunc([=]() {
        return render_window->window->size().cast<float>().eval();
    });
}
