#include <loguru.hpp>

#include <Usagi/Engine/Game/Game.hpp>
#include <Usagi/Engine/Runtime/Window/Window.hpp>
#include <Usagi/Engine/Runtime/HID/Mouse/Mouse.hpp>
#include <Usagi/Engine/Runtime/HID/Keyboard/Keyboard.hpp>

using namespace usagi;

class InputHandler
    : public KeyEventListener
    , public MouseEventListener
    , public WindowEventListener
{
    Game *mGame;
    Mouse *mMouse = mGame->mouse();
    Keyboard *mKeyboard = mGame->keyboard();
    Window *mWindow = mGame->window();

public:
    explicit InputHandler(Game *game)
        : mGame { game }
    {
        mKeyboard->addEventListener(this);
        mWindow->addEventListener(this);
    }

    ~InputHandler()
    {
        mMouse->removeEventListener(this);
        mKeyboard->removeEventListener(this);
        mWindow->removeEventListener(this);
    }

    void onMouseMove(const MousePositionEvent &e) override
    {
        LOG_F(INFO, "Mouse moved:       %f, %f",
            e.distance.x(), e.distance.y()
        );
    }

    void onMouseButtonStateChange(const MouseButtonEvent &e) override
    {
        LOG_F(INFO, "Mouse button:      %s=%d",
            to_string(e.button), e.pressed
        );
    }

    void onMouseWheelScroll(const MouseWheelEvent &e) override
    {
        LOG_F(INFO, "Mouse wheel:       %f, %f",
            e.distance.x(), e.distance.y()
        );
    }

    void onKeyStateChange(const KeyEvent &e) override
    {
        LOG_F(INFO, "Key:               %s=%d, repeated=%d",
            to_string(e.key_code), e.pressed, e.repeated
        );
        switch(e.key_code)
        {
            case KeyCode::ESCAPE:
                mWindow->close();
                break;
            case KeyCode::I:
                if(e.pressed)
                    mMouse->setImmersiveMode(
                        !mMouse->isImmersiveMode()
                    );
                break;
            // Mouse events can overwhelm the console window...
            case KeyCode::M:
                if(e.pressed)
                {
                    if(mMouse->hasEventListener(this))
                    {
                        mMouse->removeEventListener(this);
                        LOG_F(INFO, "Mouse listener removed");
                    } else
                    {
                        mMouse->addEventListener(this);
                        LOG_F(INFO, "Mouse listener added");
                    }
                }
                break;
            case KeyCode::NUM_1:
                if(e.pressed)
                    mWindow->setSize({ 640, 480 });
                break;
            case KeyCode::NUM_2:
                if(e.pressed)
                    mWindow->setSize({ 1280, 720 });
                break;
            case KeyCode::NUM_3:
                if(e.pressed)
                    mWindow->setSize({ 1920, 1080 });
                break;
            case KeyCode::NUM_4:
                if(e.pressed)
                    mWindow->setPosition({ 0, 0 });
                break;
            case KeyCode::NUM_5:
                if(e.pressed)
                    mWindow->setPosition({ 1280, 720 });
                break;
            case KeyCode::NUM_6:
                if(e.pressed)
                    mWindow->setPosition({ 1920, 1080 });
                break;
            default:
                break;
        }
    }

    void onWindowFocusChanged(const WindowFocusEvent &e) override
    {
        LOG_F(INFO, "Window focus:      %d",
            e.focused
        );
    }

    void onWindowMoveBegin(const WindowPositionEvent &e) override
    {
        LOG_F(INFO, "Window move begin");
    }

    void onWindowMoved(const WindowPositionEvent &e) override
    {
        LOG_F(INFO, "Window position:   %d, %d",
            e.position.x(), e.position.y()
        );
    }

    void onWindowMoveEnd(const WindowPositionEvent &e) override
    {
        LOG_F(INFO, "Window move end");
    }

    void onWindowResizeBegin(const WindowSizeEvent &e) override
    {
        LOG_F(INFO, "Window resize begin");
    }

    void onWindowResized(const WindowSizeEvent &e) override
    {
        LOG_F(INFO, "Window size:       %u, %u",
            e.size.x(), e.size.y()
        );
    }

    void onWindowResizeEnd(const WindowSizeEvent &e) override
    {
        LOG_F(INFO, "Window resize end");
    }
};

int main(int argc, char *argv[])
{
    Game game;
    game.initializeInput();

    InputHandler handler { &game };

    const auto window = game.window();
    while(window->isOpen()) { window->processEvents(); }
}
