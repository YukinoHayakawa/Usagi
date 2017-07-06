#pragma once

#include <memory>
#include <string>

namespace yuki
{

class RenderWindow;

/**
 * \brief Create platform-default RenderWindow.
 */
class DevicePlatform
{
public:
    virtual ~DevicePlatform()
    {
    }

    virtual std::shared_ptr<RenderWindow> createRenderWindow(const std::string &title, int width, int height) = 0;
};

}
