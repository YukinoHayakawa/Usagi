#pragma once

#include <string>
#include <memory>
#include <vector>

#include <Usagi/Core/Clock.hpp>
#include <Usagi/Core/Element.hpp>
#include <Usagi/Utility/TypeCast.hpp>

#include "Subsystem.hpp"

namespace usagi
{
struct SubsystemInfo
{
    std::string name;
    std::unique_ptr<Subsystem> subsystem;
    bool enabled = true;
};

class GameState : public Element
{
    friend class GameStateManager;

protected:
    /**
     * \brief Maintain a linked list of states. A state can use this to
     * access state below its own position on the state stack.
     */
    GameState *mPreviousState = nullptr;

    std::vector<SubsystemInfo> mSubsystems;
    Clock mClock;

    std::vector<SubsystemInfo>::iterator findSubsystemByName(
        const std::string &subsystem_name);

    void setSubsystemEnabled(
        const std::string &subsystem_name,
        bool enabled);

    Subsystem * addSubsystemPtr(
        std::string name,
        std::unique_ptr<Subsystem> subsystem);

    virtual void subsystemFilter(Subsystem *subsystem) { }

public:
    GameState(Element *parent, std::string name);

    template <typename SubsystemT>
    SubsystemT * addSubsystem(
        std::string name,
        std::unique_ptr<SubsystemT> subsystem)
    {
        auto ptr = subsystem.get();
        addSubsystemPtr(
            std::move(name),
            unique_pointer_cast<Subsystem>(std::move(subsystem))
        );
        return ptr;
    }

    void enableSubsystem(const std::string &subsystem_name);
    void disableSubsystem(const std::string &subsystem_name);

    /**
     * \brief Invoke update methods on each enabled subsystem by the order
     * of their registration.
     * \param clock
     */
    virtual void update(const Clock &clock);

    // todo also pause the clock
    virtual void pause() { }
    virtual void resume() { }
};
}
