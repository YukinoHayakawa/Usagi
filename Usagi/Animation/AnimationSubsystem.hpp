#pragma once

#include <Usagi/Game/CollectionSubsystem.hpp>

#include "AnimationComponent.hpp"

namespace usagi
{
class AnimationSubsystem : public CollectionSubsystem<AnimationComponent>
{
    std::size_t mActiveCount = 0;

public:
    void update(const Clock &clock) override;

    std::size_t activeCount() const { return mActiveCount; }
};
}
