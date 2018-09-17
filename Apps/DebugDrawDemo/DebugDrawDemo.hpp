#pragma once

#include <Usagi/Graphics/Game/GraphicalGame.hpp>

namespace usagi
{
class DebugDrawDemo : public GraphicalGame
{
public:
    explicit DebugDrawDemo(std::shared_ptr<Runtime> runtime);
};
}
