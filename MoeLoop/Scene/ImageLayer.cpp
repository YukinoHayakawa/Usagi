#include "ImageLayer.hpp"

#include <Usagi/Core/Logging.hpp>
#include <Usagi/Asset/AssetRoot.hpp>
#include <Usagi/Asset/Converter/GpuImageAssetConverter.hpp>
#include <Usagi/Transform/TransformComponent.hpp>
#include <Usagi/Runtime/Runtime.hpp>

#include <MoeLoop/Script/Lua.hpp>
#include <MoeLoop/Render/SpriteComponent.hpp>

#include "Scene.hpp"

namespace usagi::moeloop
{
ImageLayer::ImageLayer(
    Element *parent,
    std::string name,
    float y_pos,
    Scene *scene)
    : Element(parent, std::move(name))
    , mScene(scene)
{
    mTransform = addComponent<TransformComponent>();
    mTransform->setPosition({ 0, y_pos, 0 });
    mSprite = addComponent<SpriteComponent>();
}

void ImageLayer::changeImage(const std::string &asset_locator)
{
    LOG(info, "ImageLayer::changeImage {}", asset_locator);
    mSprite->texture =
        mScene->asset()->find<GpuImageAssetConverter>(
            asset_locator, mScene->runtime()->gpu());
}

void ImageLayer::exportScript(kaguya::State &vm)
{
    vm["ImageLayer"].setClass(kaguya::UserdataMetatable<ImageLayer>()
        .addFunction("changeImage", &ImageLayer::changeImage)
    );
}
}
