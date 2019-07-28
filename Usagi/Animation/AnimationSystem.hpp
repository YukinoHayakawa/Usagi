#pragma once

#include <Usagi/Game/CollectionSystem.hpp>

#include "AnimationComponent.hpp"

namespace usagi
{
class AnimationSystem final : public CollectionSystem<AnimationComponent>
{
    std::size_t mActiveCount = 0;

public:
    void update(const Clock &clock) override;

    const std::type_info & type() override
    {
        return typeid(decltype(*this));
    }

    std::size_t activeCount() const { return mActiveCount; }

    void immediatelyFinishAll();
};
}
