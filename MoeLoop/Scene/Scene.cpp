#include "Scene.hpp"

#include <Usagi/Asset/AssetRoot.hpp>

#include <MoeLoop/Script/Lua.hpp>
#include <MoeLoop/JSON/JsonAssetConverter.hpp>

#include "Expression.hpp"
#include "ImageLayer.hpp"
#include "Character.hpp"
#include "SceneConfig.hpp"

namespace usagi::moeloop
{
Scene::Scene(
    Element *parent,
    std::string asset_locator,
    Runtime *runtime,
    AssetRoot *asset)
    : Element(parent, std::move(asset_locator))
    , mRuntime(runtime)
    , mAsset(asset)
{
    mCharacters = addChild("Characters");
    mExpressions = addChild("Expressions");
    mImageLayers = addChild("ImageLayers");

    load();
}

void Scene::load()
{
    mConfig = mAsset->find<JsonAssetConverter<SceneConfig>>(name());
}

ImageLayer * Scene::createImageLayer(const std::string &name, float y_pos)
{
    return mImageLayers->addChild<ImageLayer>(name, y_pos, this);
}

Character * Scene::loadCharacter(const std::string &asset_locator)
{
    return mCharacters->addChild<Character>(asset_locator);
}

Expression * Scene::loadExpression(const std::string &asset_locator)
{
    return mExpressions->addChild<Expression>(asset_locator);
}

Vector3f Scene::getPosition(const std::string &name) const
{
    return mConfig->positions.at(name);
}

void Scene::exportScript(kaguya::State &vm)
{
    vm["Scene"].setClass(kaguya::UserdataMetatable<Scene>()
        .addFunction("createImageLayer", &Scene::createImageLayer)
        .addFunction("loadCharacter", &Scene::loadCharacter)
        .addFunction("loadExpression", &Scene::loadExpression)
        .addFunction("getPosition", &Scene::getPosition)
    );
}
}
