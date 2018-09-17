#pragma once

#include <Usagi/Graphics/Game/GraphicalGame.hpp>

namespace usagi
{
class ImGuiDemo : public GraphicalGame
{
public:
    explicit ImGuiDemo(std::shared_ptr<Runtime> runtime);
};
}
