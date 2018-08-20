#include "Game.hpp"

#include <algorithm>

#include <Usagi/Engine/Core/Event/Library/Component/ComponentAddedEvent.hpp>
#include <Usagi/Engine/Core/Event/Library/Component/PreComponentRemovalEvent.hpp>
#include <Usagi/Engine/Core/Event/Library/Component/PostComponentRemovalEvent.hpp>
#include <Usagi/Engine/Asset/AssetRoot.hpp>

#include "Subsystem.hpp"

usagi::Game::Game(Runtime *runtime)
    : mRuntime { runtime }
{
    const auto subsys_listener = [&](auto &&e) {
        for (auto &&s : mSubsystems)
        {
            s.subsystem->updateRegistry(e.source());
        }
    };

    // add listeners at root entity to allow each subsystem to examine
    // entities with updated component configurations.
    // todo: directly pass the events to the subsystems?
    mRootElement.addEventListener<ComponentAddedEvent>(subsys_listener);
    mRootElement.addEventListener<PreComponentRemovalEvent>(subsys_listener);
    mRootElement.addEventListener<PostComponentRemovalEvent>(subsys_listener);

    mRootElement.setName("ElementRoot");
    mAssetRoot = mRootElement.addChild<AssetRoot>();
}

usagi::Game::~Game()
{
}

std::vector<usagi::SubsystemInfo>::iterator usagi::Game::findSubsystemByName(
    const std::string &subsystem_name)
{
    return std::find_if(
        mSubsystems.begin(), mSubsystems.end(),
        [&](auto &s) {
            return s.name == subsystem_name;
        }
    );
}

usagi::Subsystem * usagi::Game::addSubsystemPtr(
    std::string name,
    std::unique_ptr<Subsystem> subsystem)
{
    SubsystemInfo info;
    info.name = std::move(name);
    info.subsystem = std::move(subsystem);

    // check that no existing subsystem is using the same name
    if (findSubsystemByName(info.name) != mSubsystems.end())
    {
        throw std::runtime_error("Subsystem name already used.");
    }
    const auto ret = info.subsystem.get();
    mSubsystems.push_back(std::move(info));
    return ret;
}

void usagi::Game::enableSubsystem(const std::string &subsystem_name)
{
    setSubsystemEnabled(subsystem_name, true);
}

void usagi::Game::disableSubsystem(const std::string &subsystem_name)
{
    setSubsystemEnabled(subsystem_name, false);
}

void usagi::Game::setSubsystemEnabled(
    const std::string &subsystem_name,
    bool enabled)
{
    const auto iter = findSubsystemByName(subsystem_name);
    if (iter == mSubsystems.end())
        throw std::runtime_error("No such subsystem");
    iter->enabled = enabled;
}

void usagi::Game::update(const TimeDuration &dt)
{
    for (auto &&s : mSubsystems)
    {
        if (s.enabled)
            s.subsystem->update(dt);
    }
}
