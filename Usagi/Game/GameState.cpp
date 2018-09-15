#include "GameState.hpp"

#include <algorithm>

#include <Usagi/Core/Event/Library/Component/ComponentAddedEvent.hpp>
#include <Usagi/Core/Event/Library/Component/PreComponentRemovalEvent.hpp>
#include <Usagi/Core/Event/Library/Component/PostComponentRemovalEvent.hpp>

#include "Subsystem.hpp"

usagi::GameState::GameState(Element *parent, std::string name)
    : Element(parent, std::move(name))
{
    const auto system_listener = [&](auto &&e) {
        for(auto &&s : mSubsystems)
        {
            s.subsystem->onElementComponentChanged(e.source());
        }
    };

    // add listeners at root entity to allow each subsystem to examine
    // entities with updated component configurations.
    // todo: directly pass the events to the subsystems?
    addEventListener<ComponentAddedEvent>(system_listener);
    //mRootElement.addEventListener<PreComponentRemovalEvent>(system_listener);
    addEventListener<PostComponentRemovalEvent>(system_listener);
}

std::vector<usagi::SubsystemInfo>::iterator
usagi::GameState::findSubsystemByName(const std::string &subsystem_name)
{
    return std::find_if(
        mSubsystems.begin(), mSubsystems.end(),
        [&](auto &s) { return s.name == subsystem_name; }
    );
}

void usagi::GameState::setSubsystemEnabled(
    const std::string &subsystem_name,
    bool enabled)
{
    const auto iter = findSubsystemByName(subsystem_name);
    if(iter == mSubsystems.end())
        throw std::runtime_error("No such subsystem");
    iter->enabled = enabled;
}

usagi::Subsystem * usagi::GameState::addSubsystemPtr(
    std::string name,
    std::unique_ptr<Subsystem> subsystem)
{
    SubsystemInfo info;
    info.name = std::move(name);
    info.subsystem = std::move(subsystem);

    // check that no existing subsystem is using the same name
    if(findSubsystemByName(info.name) != mSubsystems.end())
    {
        throw std::runtime_error("Subsystem name already used.");
    }
    const auto ptr = info.subsystem.get();
    mSubsystems.push_back(std::move(info));
    subsystemFilter(ptr);
    return ptr;
}

void usagi::GameState::enableSubsystem(const std::string &subsystem_name)
{
    setSubsystemEnabled(subsystem_name, true);
}

void usagi::GameState::disableSubsystem(const std::string &subsystem_name)
{
    setSubsystemEnabled(subsystem_name, false);
}

void usagi::GameState::update(const Clock &clock)
{
    for(auto &&s : mSubsystems)
    {
        if(s.enabled)
            s.subsystem->update(clock);
    }
}
