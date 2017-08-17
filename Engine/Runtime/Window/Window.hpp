#pragma once

namespace yuki
{

/**
* \brief Window is the interface between the player and the engine. It is mainly for
* receive user inputs, but can also be the target of graphical rendering, if the it
* provide a platform-specific handle for a GraphicsDevice.
*/
class Window
{
public:
    virtual ~Window() = default;

    virtual void showWindow(bool show) = 0;

    virtual void processEvents() = 0;
};

}
