#include "ImageFrame.hpp"

#include <Usagi/Transform/TransformComponent.hpp>

#include <MoeLoop/Render/SpriteComponent.hpp>

namespace usagi::moeloop
{
ImageFrame::ImageFrame(Element *parent, std::string name)
    : PredefinedElement(parent, std::move(name))
{
}
}
