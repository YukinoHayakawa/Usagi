#pragma once

#include <Usagi/Game/CollectionSystem.hpp>

#include "AnimationComponent.hpp"

namespace usagi
{
class AnimationSystem : public CollectionSystem<AnimationComponent>
{
    std::size_t mActiveCount = 0;

public:
    void update(const Clock &clock) override;

    std::size_t activeCount() const { return mActiveCount; }
};
}
