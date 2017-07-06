#pragma once

#include <memory>
#include <string>

#include <Usagi/Engine/Utility/Exception.hpp>

namespace yuki
{

YUKI_DECL_RUNTIME_EXCEPTION(RenderingContextCreationFailure);

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
