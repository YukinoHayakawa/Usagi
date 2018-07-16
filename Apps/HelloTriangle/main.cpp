#include <Usagi/Engine/Game/Game.hpp>
#include <Usagi/Engine/Runtime/Window/Window.hpp>

using namespace usagi;

int main(int argc, char *argv[])
{
    Game game;
    game.initializeInput();
    game.initializeGraphics();

    const auto window = game.window();
    while(window->isOpen()) { window->processEvents(); }
}
