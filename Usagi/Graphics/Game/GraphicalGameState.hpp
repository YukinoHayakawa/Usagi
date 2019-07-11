#pragma once

#include <Usagi/Game/GameState.hpp>

namespace usagi
{
class GraphicsCommandList;
class GraphicalGame;
class RenderableSystem;

class GraphicalGameState : public GameState
{
protected:
    GraphicalGame *mGame;

    using IndexedRenderable = std::pair<std::size_t, RenderableSystem*>;
    std::vector<IndexedRenderable> mRenderableSystems;
    std::vector<std::shared_ptr<GraphicsCommandList>> mCommandLists;

    void subsystemFilter(System *subsystem) override;

public:
    GraphicalGameState(Element *parent, std::string name, GraphicalGame *game);

    void update(const Clock &clock) override;
};
}
