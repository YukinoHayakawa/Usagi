#include "Game.hpp"

#include <algorithm>

#include <Usagi/Engine/Core/Event/Library/AddComponentEvent.hpp>
#include <Usagi/Engine/Runtime/Window/Window.hpp>
#include <Usagi/Engine/Graphics/GpuDevice.hpp>

#include "Subsystem.hpp"

usagi::Game::Game()
{
    // add listeners at root entity to allow each subsystem to examine
    // entities with updated component configurations.
    mRootEntity.addEventListener<AddComponentEvent>(
        [&](auto &&e) {
            for(auto &&s : mSubsystems)
            {
                s.subsystem->updateRegistry(e.source());
            }
        }
    );
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

usagi::Subsystem * usagi::Game::addSubsystem(SubsystemInfo subsystem)
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

void usagi::Game::enableSubsystem(const std::string &subsystem_name)
{
	setSubsystemEnabled(subsystem_name, true);
}

void usagi::Game::disableSubsystem(const std::string &subsystem_name)
{
	setSubsystemEnabled(subsystem_name, false);
}

void usagi::Game::setSubsystemEnabled(
    const std::string &subsystem_name, bool enabled)
{
    const auto iter = findSubsystemByName(subsystem_name);
    if(iter == mSubsystems.end())
        throw std::runtime_error("No such subsystem");
    iter->enabled = enabled;
}

void usagi::Game::update(const std::chrono::seconds &dt)
{
    for(auto &&s : mSubsystems)
    {
        if(s.enabled)
            s.subsystem->update(dt);
    }
}
