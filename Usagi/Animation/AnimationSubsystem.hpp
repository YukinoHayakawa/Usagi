#pragma once

#include <Usagi/Game/CollectionSubsystem.hpp>

#include "AnimationComponent.hpp"

namespace usagi
{
class AnimationSubsystem : public CollectionSubsystem<AnimationComponent>
{
public:
    void update(const Clock &clock) override;
};
}
