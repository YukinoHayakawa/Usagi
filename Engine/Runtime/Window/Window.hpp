#pragma once

#include <eigen3/Eigen/Dense>

namespace yuki
{

/**
* \brief Window is the interface between the player and the engine. It is mainly
* for receiving user inputs, but can also be the target of graphical rendering.
*/
class Window
{
public:
    virtual ~Window() = default;

    virtual void showWindow(bool show) = 0;
    virtual bool isWindowActive() const = 0;
    virtual bool isWindowOpen() const = 0;
    virtual Eigen::Vector2f getWindowSize() const = 0;
    virtual void setTitle(const std::string &title) = 0;

    virtual void processEvents() = 0;
};

}
