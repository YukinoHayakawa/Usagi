#include "ImageLayer.hpp"

#include <Usagi/Core/Logging.hpp>
#include <Usagi/Asset/AssetRoot.hpp>
#include <Usagi/Asset/Converter/GpuImageAssetConverter.hpp>
#include <Usagi/Transform/TransformComponent.hpp>
#include <Usagi/Runtime/Runtime.hpp>

#include <MoeLoop/Script/Lua.hpp>

#include "Scene.hpp"

namespace usagi::moeloop
{
ImageLayer::ImageLayer(
    Element *parent,
    std::string name,
    float y_pos,
    Scene *scene)
    : TransitionableImage(parent, std::move(name))
    , mScene(scene)
{
    comp<TransformComponent>()->setPosition({ 0, y_pos, 0 });
}

void ImageLayer::changeImage(const std::string &name)
{
    LOG(info, "ImageLayer::changeImage {}", name);
    const auto texture = mScene->asset()->res<GpuImageAssetConverter>(
        "images/" + name, mScene->runtime()->gpu());
    switchImage(1.0, "linear", texture);
    comp<TransformComponent>()->setOffset({
        0, 0, static_cast<float>(texture->size().y())
    });
}

void ImageLayer::exportScript(kaguya::State &vm)
{
    vm["ImageLayer"].setClass(kaguya::UserdataMetatable<ImageLayer>()
        .addFunction("changeImage", &ImageLayer::changeImage)
    );
}
}
