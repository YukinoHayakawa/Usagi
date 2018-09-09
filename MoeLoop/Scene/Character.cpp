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
    : TransitionableImage(parent, std::move(name))
{
}

void Character::setPosition(const Vector3f &position)
{
    comp<TransformComponent>()->setPosition(position);
}

void Character::changeExpression(Expression *expr)
{
    // todo use scene default
    switchImage(1.0, "linear", expr->texture());
    const auto o = expr->origin();
    comp<SpriteComponent>()->layers[1].offset = -o;
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
    Animation ani;
    ani.duration = 0.5;
    ani.animation_func = [
        this,
        start_pos = comp<TransformComponent>()->position(),
        position
    ](const double t) {
        setPosition(lerp(t, start_pos, position));
    };
    comp<AnimationComponent>()->add(std::move(ani));
}

void Character::say(const std::string &text)
{
    LOG(info, "{}: {}", name(), text);
}

void Character::enterScene(
    Expression *expr,
    const Vector3f &position)
{
    changeExpression(expr);
    // move(position);
    setPosition(position);
}

void Character::exitScene()
{
    switchImage(1.0, "linear", { });
}

void Character::pretendSay(
    const std::string &fake_name,
    const std::string &text)
{
    LOG(info, "{}({}): {}", fake_name, name(), text);
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
