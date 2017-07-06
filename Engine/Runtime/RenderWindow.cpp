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

void yuki::RenderWindow::swapBuffer()
{
    mGI->swapBuffer();
}

void yuki::RenderWindow::setCurrent()
{
    mGI->setCurrent();
}

void yuki::RenderWindow::processEvents()
{
    mWindow->processEvents();
}
