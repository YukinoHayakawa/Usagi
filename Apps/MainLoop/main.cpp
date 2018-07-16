#include <loguru.hpp>

#include <Usagi/Engine/Game/Game.hpp>
#include <Usagi/Engine/Runtime/Window/Window.hpp>
#include <Usagi/Engine/Runtime/HID/Mouse/Mouse.hpp>
#include <Usagi/Engine/Runtime/HID/Keyboard/Keyboard.hpp>

using namespace usagi;

class InputHandler
    : public KeyEventListener, public MouseEventListener
{
    Game *mGame;

public:
    explicit InputHandler(Game *game)
        : mGame { game }
    {
    }

    void onMouseMove(const MousePositionEvent &e) override
    {
        LOG_F(INFO, "Mouse moved:   %f, %f",
            e.cursor_position_delta.x(), e.cursor_position_delta.y()
        );
    }

    void onMouseButtonStateChange(const MouseButtonEvent &e) override
    {
        LOG_F(INFO, "Mouse button:  %s=%d",
            to_string(e.button), e.pressed
        );
    }

    void onMouseWheelScroll(const MouseWheelEvent &e) override
    {
        LOG_F(INFO, "Mouse wheel:   %d, %d",
            0, e.distance
        );
    }

    void onKeyStateChange(const KeyEvent &e) override
    {
        LOG_F(INFO, "Key:           %s=%d, repeated=%d",
            to_string(e.key_code), e.pressed, e.repeated
        );
        switch(e.key_code)
        {
            case KeyCode::ESCAPE:
                mGame->window()->close();
                break;
            case KeyCode::I:
                if(e.pressed)
                    mGame->mouse()->setImmersiveMode(
                        !mGame->mouse()->isImmersiveMode()
                    );
                break;
            default:
                break;
        }
    }
};

int main(int argc, char *argv[])
{
    Game game;
    game.initializeDevices();

    InputHandler handler { &game };
    game.keyboard()->addEventListener(&handler);
    game.mouse()->addEventListener(&handler);

    const auto window = game.window();
    while(window->isOpen()) { window->processEvents(); }
}
