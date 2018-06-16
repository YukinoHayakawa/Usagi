#pragma once

#include <vector>
#include <memory>
#include <chrono>

#include <Usagi/Engine/Utility/Noncopyable.hpp>
#include <Usagi/Engine/Entity/Entity.hpp>

namespace yuki
{
class Subsystem;

struct SubsystemInfo
{
    std::string name;
    std::unique_ptr<Subsystem> subsystem;
    bool enabled = true;
};

/**
 * \brief Collection of subsystems and entities.
 * Note that the order of inserting subsystems is the same as they
 * are updated.
 */
class Game final : Noncopyable
{
public:
    Game();

    /**
     * \brief
     * \param subsystem 
     */
    void addSubsystem(SubsystemInfo subsystem);
    void setSubsystemEnabled(
        const std::string &subsystem_name, bool enabled);

    Entity * getRootEntity();

    /**
    * \brief Invoke update methods on each enabled subsystem by the order
    * of their registration.
    * \param dt The elspased time from last frame.
    */
    void update(const std::chrono::seconds &dt);

private:
    std::vector<SubsystemInfo> mSubsystems;

    std::vector<SubsystemInfo>::iterator findSubsystemByName(
        const std::string &subsystem_name);

    Entity mRoot { nullptr };
};
}
