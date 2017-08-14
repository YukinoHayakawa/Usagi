#include <Usagi/Engine/Time/Clock.hpp>
#include <Usagi/Engine/Runtime/Subsystem/DebugDrawManager.hpp>
#include <Usagi/Engine/Runtime/HID/Mouse/MouseEventListener.hpp>
#include <Usagi/Engine/Runtime/HID/Keyboard/KeyEventListener.hpp>
#include <Usagi/Engine/Runtime/HID/Keyboard/KeyCode.hpp>
#include <Usagi/Engine/Extension/Win32/WGLContext.hpp>
#include <Usagi/Engine/Extension/OpenGL/OpenGLGraphicsDevice.hpp>

// dirty one last
#include <Usagi/Engine/Extension/Win32/Win32Window.hpp>

class HIDDebugListener : public yuki::MouseEventListener, public yuki::KeyEventListener
{
public:
    void onKeyStateChange(const KeyEvent &e) override
    {
        std::cout << getKeyName(e.keyCode) << (e.pressed ? " pressed" : " released") << std::endl;
    }

    void onMouseMove(const yuki::MousePositionEvent &e) override
    {
        std::cout << "mouse pos " << e.cursorWindowPos.transpose() << " delta " << e.cursorPosDelta.transpose() << std::endl;
    }

    void onMouseButtonStateChange(const yuki::MouseButtonEvent &e) override
    {
        std::cout << getMouseButtonName(e.button) << (e.pressed ? " pressed" : " released") << std::endl;
    }

    void onMouseWheelScroll(const yuki::MouseWheelEvent &e) override
    {
        std::cout << "scroll " << e.distance << std::endl;
    }
};

int main(int argc, char *argv[])
{
    using namespace yuki;

    try
    {
        auto window = std::make_shared<Win32Window>("UsagiEditor", 1280, 720);
        auto device = std::make_shared<OpenGLGraphicsDevice>(std::make_shared<WGLContext>(window->getDeviceContext()));
        DebugDrawManager debug_draw_manager(*device.get());
        Clock master_master;
        auto input_handler = std::make_shared<HIDDebugListener>();

        window->addMouseEventListener(input_handler);
        window->addKeyEventListener(input_handler);
        window->show();
        device->setContextCurrent();

        while(true)
        {
            master_master.tick();
            window->processEvents();

            device->clearCurrentFrameBuffer();
            debug_draw_manager.render(*device.get(), master_master);

            device->swapFrameBuffers();
        }
    }
    catch(std::exception &)
    {
        throw;
    }
}
