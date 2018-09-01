#include "Character.hpp"

#include <Usagi/Transform/TransformComponent.hpp>
#include <Usagi/Animation/AnimationComponent.hpp>
#include <Usagi/Core/Logging.hpp>

#include <MoeLoop/Render/SpriteComponent.hpp>
#include <MoeLoop/Script/Lua.hpp>

#include "Expression.hpp"

namespace usagi::moeloop
{
Character::Character(Element *parent, std::string name)
    : Element(parent, std::move(name))
{
    mTransform = addComponent<TransformComponent>();
    mSprite = addComponent<SpriteComponent>();
    mAnimation = addComponent<AnimationComponent>();
}

void Character::changeExpression(Expression *expr)
{
    mCurrentExpression = expr;
    const auto o = mCurrentExpression->origin();
    mTransform->setOffset({ o.x(), 0, o.y() });
    mSprite->texture = mCurrentExpression->texture();
    mSprite->uv_rect = mCurrentExpression->textureUvRect();
}

void Character::showName(bool show)
{
    mShowName = show;
}

void Character::showAvatar(bool show)
{
    mShowAvatar = show;
}

void Character::move(const Vector3f &position)
{
    mTransform->setPosition(position);
}

void Character::say(const std::string &text)
{
    LOG(info, "{}({}): {}", name(), mCurrentExpression->name(), text);
}

void Character::enterScene(
    Expression *expr,
    const Vector3f &position)
{
    changeExpression(expr);
    mSprite->show = true;
    mTransform->setPosition(position);
}

void Character::exitScene()
{
    LOG(info, "{} exits scene", name());
}

void Character::pretendSay(
    const std::string &fake_name,
    const std::string &text)
{
    LOG(info, "{}({}): {}", fake_name, mCurrentExpression->name(), text);
}

void Character::exportScript(kaguya::State &vm)
{
    vm["Character"].setClass(kaguya::UserdataMetatable<Character>()
        .addFunction("enterScene", &Character::enterScene)
        .addFunction("exitScene", &Character::exitScene)
        .addFunction("changeExpression", &Character::changeExpression)
        .addFunction("move", &Character::move)
        .addFunction("say", &Character::say)
        .addFunction("pretendSay", &Character::pretendSay)
    );
}
}
