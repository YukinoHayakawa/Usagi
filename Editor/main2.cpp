#include <Usagi/Engine/Runtime/Window/Win32Window.hpp>
#include <Usagi/Engine/Runtime/GraphicsDevice/OpenGL/Context/WGLContext.hpp>
#include <Usagi/Engine/Runtime/GraphicsDevice/OpenGL/OpenGLGraphicsDevice.hpp>
#include <Usagi/Engine/Runtime/Subsystem/DebugDrawManager.hpp>
#include <Usagi/Engine/Runtime/Clock.hpp>



int main(int argc, char *argv[])
{
    using namespace yuki;

    try
    {
        auto window = std::make_shared<Win32Window>("UsagiEditor", 1280, 720);
        auto device = std::make_shared<OpenGLGraphicsDevice>(std::make_shared<WGLContext>(window->getDeviceContext()));

        window->show();
        device->setContextCurrent();

        DebugDrawManager dd(*device.get());
        Clock master;

        while(true)
        {
            master.tick();

            window->processEvents();

            device->clearCurrentFrameBuffer();

            dd.render(*device.get(), master);
            
            device->swapFrameBuffers();
        }
    }
    catch(std::exception &)
    {
        throw;
    }

    return 0;
}
