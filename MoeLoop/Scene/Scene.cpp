#include "Scene.hpp"

#include <MoeLoop/Script/Lua.hpp>

#include "Expression.hpp"
#include "ImageLayer.hpp"
#include "Character.hpp"

namespace usagi::moeloop
{
Scene::Scene(
    Element *parent,
    std::string name,
    const std::uint32_t width,
    const std::uint32_t height)
    : Element(parent, std::move(name))
    , mSize(width, height)
{
}

ImageLayer * Scene::createImageLayer(const std::string &name, float y_pos)
{
    return addChild<ImageLayer>(name, y_pos);
}

Character * Scene::createCharacter(const std::string &name)
{
    return addChild<Character>(name);
}

std::shared_ptr<Expression> Scene::createExpression(const std::string &name)
{
    return std::make_shared<Expression>(name);
}

Vector3f Scene::createPosition(
    const std::string &name,
    const float x,
    const float y,
    const float z)
{
    return { x, y, z };
}

void Scene::exportScript(kaguya::State &vm)
{
    vm["Scene"].setClass(kaguya::UserdataMetatable<Scene>()
        .addFunction("createImageLayer", &Scene::createImageLayer)
        .addFunction("createCharacter", &Scene::createCharacter)
        .addFunction("createExpression", &Scene::createExpression)
        .addFunction("createPosition", &Scene::createPosition)
    );
}
}
