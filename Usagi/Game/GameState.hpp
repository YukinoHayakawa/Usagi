#pragma once

#include <string>
#include <memory>
#include <vector>

#include <Usagi/Core/Clock.hpp>
#include <Usagi/Core/Element.hpp>
#include <Usagi/Utility/TypeCast.hpp>

#include "System.hpp"

namespace usagi
{
struct SystemInfo
{
    std::string name;
    std::unique_ptr<System> subsystem;
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

    std::vector<SystemInfo> mSystems;
    Clock mClock;

    std::vector<SystemInfo>::iterator findSystemByName(
        const std::string &subsystem_name);

    void setSystemEnabled(
        const std::string &subsystem_name,
        bool enabled);

    System * addSystemPtr(
        std::string name,
        std::unique_ptr<System> subsystem);

    virtual void subsystemFilter(System *subsystem) { }

public:
    GameState(Element *parent, std::string name);

    template <typename SystemT>
    SystemT * addSystem(
        std::string name,
        std::unique_ptr<SystemT> subsystem)
    {
        auto ptr = subsystem.get();
        addSystemPtr(
            std::move(name),
            unique_pointer_cast<System>(std::move(subsystem))
        );
        return ptr;
    }

    template <typename SystemT, typename... Args>
    SystemT * addSystem(std::string name, Args &&... args)
    {
        auto sys = std::make_unique<SystemT>(std::forward<Args>(args)...);
        return addSystem(std::move(name), std::move(sys));
    }

    void enableSystem(const std::string &subsystem_name);
    void disableSystem(const std::string &subsystem_name);

    /**
     * \brief Invoke update methods on each enabled subsystem by the order
     * of their registration.
     * \param clock
     */
    virtual void update(const Clock &clock);

    // todo also pause the clock
    virtual void pause() { }

    /**
     * \brief Invoked when the state should start to run. Can either be when
     * the state is put on the stack top for the first time or when other
     * states are popped out.
     */
    virtual void resume() { }
};
}
