#include "RenderWindow.hpp"

yuki::RenderWindow::RenderWindow(std::shared_ptr<Window> window, std::shared_ptr<GraphicsInterface> graphics_interface)
    : mWindow { std::move(window) }
    , mGI { std::move(graphics_interface) }
{
}

void yuki::RenderWindow::show()
{
    mWindow->show();
}

void yuki::RenderWindow::hide()
{
    mWindow->hide();
}
