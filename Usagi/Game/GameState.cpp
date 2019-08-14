#include "GameState.hpp"

#include <algorithm>

#include <Usagi/Core/Event/Library/Component/ComponentAddedEvent.hpp>
#include <Usagi/Core/Event/Library/Component/PreComponentRemovalEvent.hpp>
#include <Usagi/Core/Event/Library/Component/PostComponentRemovalEvent.hpp>

#include "System.hpp"

usagi::GameState::GameState(Element *parent, std::string name)
    : Element(parent, std::move(name))
{
    const auto system_listener = [&](auto &&e) {
        for(auto &&s : mSystems)
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

std::vector<usagi::SystemInfo>::iterator
usagi::GameState::findSystemByName(const std::string &subsystem_name)
{
    return std::find_if(
        mSystems.begin(), mSystems.end(),
        [&](auto &s) { return s.name == subsystem_name; }
    );
}

void usagi::GameState::setSystemEnabled(
    const std::string &subsystem_name,
    bool enabled)
{
    const auto iter = findSystemByName(subsystem_name);
    if(iter == mSystems.end())
        USAGI_THROW(std::runtime_error("No such subsystem"));
    iter->enabled = enabled;
}

usagi::System * usagi::GameState::addSystemPtr(
    std::string name,
    std::unique_ptr<System> subsystem)
{
    SystemInfo info;
    info.name = std::move(name);
    info.subsystem = std::move(subsystem);

    // check that no existing subsystem is using the same name
    if(findSystemByName(info.name) != mSystems.end())
    {
        USAGI_THROW(std::runtime_error("System name already used."));
    }
    const auto ptr = info.subsystem.get();
    mSystems.push_back(std::move(info));
    subsystemFilter(ptr);
    return ptr;
}

void usagi::GameState::enableSystem(const std::string &subsystem_name)
{
    setSystemEnabled(subsystem_name, true);
}

void usagi::GameState::disableSystem(const std::string &subsystem_name)
{
    setSystemEnabled(subsystem_name, false);
}

void usagi::GameState::update(const Clock &clock)
{
    for(auto &&s : mSystems)
    {
        if(s.enabled)
            s.subsystem->update(clock);
    }
}
