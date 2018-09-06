#pragma once

#include <Usagi/Core/PredefinedElement.hpp>

namespace usagi
{
struct TransformComponent;
}

namespace usagi::moeloop
{
struct SpriteComponent;

class ImageFrame
    : public PredefinedElement<TransformComponent, SpriteComponent>
{
public:
    ImageFrame(Element *parent, std::string name);
};
}
