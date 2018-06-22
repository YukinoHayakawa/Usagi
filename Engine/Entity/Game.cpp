#include "Game.hpp"

#include <algorithm>

#include <Usagi/Engine/Event/Library/AddComponentEvent.hpp>

#include "Subsystem.hpp"

yuki::Game::Game()
{
    // add listeners at root entity to allow each subsystem to examine
    // entities with updated component configurations.
    mRoot.addEventListener<AddComponentEvent>(
        [&](auto &&e) {
            for(auto &&s : mSubsystems)
            {
                s.subsystem->updateRegistry(e.source());
            }
        }
    );
}

std::vector<yuki::SubsystemInfo>::iterator yuki::Game::findSubsystemByName(
    const std::string &subsystem_name)
{
    return std::find_if(
        mSubsystems.begin(), mSubsystems.end(),
        [&](auto &s) {
            return s.name == subsystem_name;
        }
    );
}

yuki::Subsystem * yuki::Game::addSubsystem(SubsystemInfo subsystem)
{
    // check that no exisiting subsystem is using the same name
    if(findSubsystemByName(subsystem.name) != mSubsystems.end())
    {
        throw std::runtime_error("Subsystem name already used.");
    }
    const auto ret = subsystem.subsystem.get();
    mSubsystems.push_back(std::move(subsystem));
    return ret;
}

void yuki::Game::setSubsystemEnabled(
    const std::string &subsystem_name, bool enabled)
{
    const auto iter = findSubsystemByName(subsystem_name);
    if(iter == mSubsystems.end())
        throw std::runtime_error("No such subsystem");
    iter->enabled = enabled;
}

yuki::Entity * yuki::Game::getRootEntity()
{
    return &mRoot;
}

void yuki::Game::update(const std::chrono::seconds &dt)
{
    for(auto &&s : mSubsystems)
    {
        if(s.enabled)
            s.subsystem->update(dt);
    }
}
