#pragma once

#include <Usagi/Graphics/Game/GraphicalGame.hpp>
#include <Usagi/Interactive/InputMapping.hpp>

#include "Script/Lua.hpp"

namespace usagi
{
class GameState;
}

namespace usagi::moeloop
{
class Scene;
class SceneState;

class MoeLoopGame : public GraphicalGame
{
protected:
    kaguya::State mLuaContext;

    InputMapping mInputMapping;

    void bindScript();
    void setupInput();

public:
    explicit MoeLoopGame(std::shared_ptr<Runtime> runtime);
    ~MoeLoopGame();

    /**
     * \brief Push InitGameState and execute init script.
     */
    virtual void init();

    InputMapping * inputMapping() { return &mInputMapping; }
    kaguya::State * luaContext() { return &mLuaContext; }

    static void unimplemented(const std::string &msg);
    void addFilesystemPackage(std::string name, const std::string &path);

    void changeState(GameState *state);
    void pushState(GameState *state);
    void popState();

    virtual SceneState * createSceneState(const std::string &name) = 0;
    Scene *currentScene() const;
};
}
