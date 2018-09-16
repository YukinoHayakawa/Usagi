#pragma once

#include <Usagi/Graphics/Game/GraphicalGame.hpp>

namespace usagi
{
class NuklearDemo : public GraphicalGame
{
public:
    explicit NuklearDemo(std::shared_ptr<Runtime> runtime);
    ~NuklearDemo();
};
}
