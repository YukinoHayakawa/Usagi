#pragma once

#include "DevicePlatform.hpp"

namespace yuki
{

class Win32DevicePlatform : public DevicePlatform
{
public:
    std::shared_ptr<RenderWindow> createRenderWindow(const std::string &title, int width, int height) override;
};

}
