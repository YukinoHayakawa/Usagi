#pragma once

#include <memory>
#include <vector>
#include <chrono>

#include <Usagi/Engine/Utility/Noncopyable.hpp>
#include <Usagi/Engine/Entity/Entity.hpp>

namespace usagi
{
class Subsystem;
class Window;
class GraphicsDevice;

struct SubsystemInfo
{
	std::string name;
	std::unique_ptr<Subsystem> subsystem;
	bool enabled = true;
};

class Game : Noncopyable
{
    std::unique_ptr<Window> mWindow;
    std::unique_ptr<GraphicsDevice> mGraphicsDevice;
    std::vector<SubsystemInfo> mSubsystems;
	Entity mRootEntity { nullptr };

	std::vector<SubsystemInfo>::iterator findSubsystemByName(
		const std::string &subsystem_name);

	void setSubsystemEnabled(
		const std::string &subsystem_name, bool enabled);

public:
   Game();

    /**
     * \brief
     * \param subsystem 
     */
    Subsystem * addSubsystem(SubsystemInfo subsystem);

    void enableSubsystem(const std::string &subsystem_name);
    void disableSubsystem(const std::string &subsystem_name);

    Entity * rootEntity() { return &mRootEntity; }

    /**
    * \brief Invoke update methods on each enabled subsystem by the order
    * of their registration.
    * \param dt The elapsed time from last frame.
    */
    void update(const std::chrono::seconds &dt);
};
}
