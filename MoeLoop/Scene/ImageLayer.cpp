#include "ImageLayer.hpp"

#include <Usagi/Core/Logging.hpp>

#include <MoeLoop/Script/Lua.hpp>

namespace usagi::moeloop
{
ImageLayer::ImageLayer(Element *parent, std::string name, float y_pos)
    : Element(parent, std::move(name))
{
}

void ImageLayer::changeImage(const std::string &name)
{
    LOG(info, "ImageLayer::changeImage {}", name);
}

void ImageLayer::exportScript(kaguya::State &vm)
{
    vm["ImageLayer"].setClass(kaguya::UserdataMetatable<ImageLayer>()
        .addFunction("changeImage", &ImageLayer::changeImage)
    );
}
}
