#include "Game.hpp"

#include <algorithm>

#include <Usagi/Engine/Event/AddComponentEvent.hpp>

#include "Subsystem.hpp"

yuki::Game::Game()
{
    // add listeners at root entity to allow each subsystem to examine
    // entities with updated component configurations.
    mRoot.addEventListener<AddComponentEvent>([&](auto &&e)
    {
        for(auto &&s : mSubsystems)
        {
            s.subsystem->updateRegistry(e.source());
        }
    });
}

void yuki::Game::addSubsystem(SubsystemInfo subsystem)
{
    // check that no exisiting subsystem is using the same name
    if(std::find_if(mSubsystems.begin(), mSubsystems.end(), [&](auto &s)
    {
        return s.name == subsystem.name;
    }) != mSubsystems.end())
    {
        throw std::runtime_error("Subsystem name already used.");
    }
    mSubsystems.push_back(std::move(subsystem));
}

yuki::Entity * yuki::Game::getRootEntity()
{
    return &mRoot;
}

void yuki::Game::update(const std::chrono::milliseconds &dt)
{
    for(auto &&s : mSubsystems)
    {
        s.subsystem->update(dt);
    }
}
