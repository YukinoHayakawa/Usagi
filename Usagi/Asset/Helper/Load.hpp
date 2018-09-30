#pragma once

#include <memory>
#include <string>

namespace usagi
{
class Game;
class GpuImage;

std::shared_ptr<GpuImage> loadTexture(Game *game, const std::string &locator);
}
